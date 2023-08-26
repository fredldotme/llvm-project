add_lldb_library(lldbPluginPlatformWasm PLUGIN
  PlatformRemoteWasmServer.cpp

   LINK_LIBS
    lldbBreakpoint
    lldbCore
    lldbHost
    lldbTarget
    lldbPluginProcessUtility
  )
