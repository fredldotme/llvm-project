#ifndef LLDB_SOURCE_PLUGINS_PLATFORM_GDB_SERVER_PLATFORMREMOTEWASMSERVER_H
#define LLDB_SOURCE_PLUGINS_PLATFORM_GDB_SERVER_PLATFORMREMOTEWASMSERVER_H

#include "Plugins/Platform/gdb-server/PlatformRemoteGDBServer.h"
#include "lldb/Target/Platform.h"

namespace lldb_private {
namespace platform_wasm_server {

class PlatformRemoteWASMServer : public lldb_private::platform_gdb_server::PlatformRemoteGDBServer{

public:
  static void Initialize();

  static void Terminate();

  static lldb::PlatformSP CreateInstance(bool force, const ArchSpec *arch);

  static ConstString GetPluginNameStatic();

  static const char *GetDescriptionStatic();

  size_t ConnectToWaitingProcesses(lldb_private::Debugger &debugger,
                                   lldb_private::Status &error) override;
  
  bool GetSupportedArchitectureAtIndex(uint32_t idx, ArchSpec &arch);

  llvm::StringRef GetPluginName() override;

  PlatformRemoteWASMServer();

};

} // namespace platform_wasm_server
} // namespace lldb_private

#endif
