#include "PlatformRemoteWasmServer.h"
#include "lldb/Host/Config.h"

#include "lldb/Breakpoint/BreakpointLocation.h"
#include "lldb/Core/Debugger.h"
#include "lldb/Core/Module.h"
#include "lldb/Core/ModuleList.h"
#include "lldb/Core/ModuleSpec.h"
#include "lldb/Core/PluginManager.h"
#include "lldb/Core/StreamFile.h"
#include "lldb/Host/ConnectionFileDescriptor.h"
#include "lldb/Host/Host.h"
#include "lldb/Host/HostInfo.h"
#include "lldb/Host/PosixApi.h"
#include "lldb/Target/Process.h"
#include "lldb/Target/Target.h"
#include "lldb/Utility/FileSpec.h"
#include "lldb/Utility/Log.h"
#include "lldb/Utility/ProcessInfo.h"
#include "lldb/Utility/Status.h"
#include "lldb/Utility/StreamString.h"
#include "lldb/Utility/UriParser.h"

#include "Plugins/Process/Utility/GDBRemoteSignals.h"

using namespace lldb;
using namespace lldb_private;
using namespace lldb_private::platform_wasm_server;

LLDB_PLUGIN_DEFINE_ADV(PlatformRemoteWASMServer, PlatformWasm)

static bool g_initialized = false;

void PlatformRemoteWASMServer::Initialize() {
  Platform::Initialize();

  if (!g_initialized) {
    g_initialized = true;
    PluginManager::RegisterPlugin(
        PlatformRemoteWASMServer::GetPluginNameStatic().GetStringRef(),
        PlatformRemoteWASMServer::GetDescriptionStatic(),
        PlatformRemoteWASMServer::CreateInstance);
  }
}

void PlatformRemoteWASMServer::Terminate() {
  if (g_initialized) {
    g_initialized = false;
    PluginManager::UnregisterPlugin(PlatformRemoteWASMServer::CreateInstance);
  }

  Platform::Terminate();
}

PlatformSP PlatformRemoteWASMServer::CreateInstance(bool force,
                                                    const ArchSpec *arch) {
  Log *log(GetLogIfAllCategoriesSet(LIBLLDB_LOG_PLATFORM));
  if (log) {
    const char *arch_name;
    if (arch && arch->GetArchitectureName())
      arch_name = arch->GetArchitectureName();
    else
      arch_name = "<null>";

    const char *triple_cstr =
        arch ? arch->GetTriple().getTriple().c_str() : "<null>";

    LLDB_LOGF(log, "PlatformRemoteWASMServer::%s(force=%s, arch={%s,%s})",
              __FUNCTION__, force ? "true" : "false", arch_name, triple_cstr);
  }

  bool create = force;
  if (!create && arch && arch->IsValid()) {
    const llvm::Triple &triple = arch->GetTriple();
    if (arch->GetMachine() == llvm::Triple::wasm32 &&
        triple.getOS() == llvm::Triple::WASI) {
      create = true;
    }
  }

  if (create) {
    if (log)
      LLDB_LOGF(log, "PlatformRemoteWASMServer::%s() creating platform",
                __FUNCTION__);
    return PlatformSP(new PlatformRemoteWASMServer());
  }

  if (log)
    LLDB_LOGF(log,
              "PlatformRemoteWASMServer::%s() aborting creation of platform",
              __FUNCTION__);
  return PlatformSP();
}

ConstString PlatformRemoteWASMServer::GetPluginNameStatic() {
  static ConstString g_name("remote-wasm-server");
  return g_name;
}

llvm::StringRef PlatformRemoteWASMServer::GetPluginName() {
  return GetPluginNameStatic().GetStringRef();
}

const char *PlatformRemoteWASMServer::GetDescriptionStatic() {
  return "A platform that uses the GDB remote protocol as the communication "
         "transport for Wasm Runtime";
}

size_t PlatformRemoteWASMServer::ConnectToWaitingProcesses(Debugger &debugger,
                                                          Status &error) {
  std::vector<std::string> connection_urls;
  GetPendingGdbServerList(connection_urls);

  for (size_t i = 0; i < connection_urls.size(); ++i) {
    ConnectProcess(connection_urls[i].c_str(), "wasm", debugger, nullptr, error);
    if (error.Fail())
      return i; // We already connected to i process succsessfully
  }
  return connection_urls.size();
}

bool PlatformRemoteWASMServer::GetSupportedArchitectureAtIndex(uint32_t idx,
                                                              ArchSpec &arch) {
  ArchSpec remote_arch = m_gdb_client.GetSystemArchitecture();
  if (idx == 0) {
    arch = remote_arch;
    return arch.IsValid();
  } else if (idx == 1 && remote_arch.IsValid() &&
             remote_arch.GetTriple().getOS() == llvm::Triple::WASI) {
    return arch.IsValid();
  }
  return false;
}

/// Default Constructor
PlatformRemoteWASMServer::PlatformRemoteWASMServer()
    : PlatformRemoteGDBServer()
    {
    }
