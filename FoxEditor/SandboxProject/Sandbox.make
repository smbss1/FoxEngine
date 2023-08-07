# GNU Make project makefile autogenerated by Premake

ifndef config
  config=debug
endif

ifndef verbose
  SILENT = @
endif

.PHONY: clean prebuild prelink

ifeq ($(config),debug)
  CSC = csc
  RESGEN = resgen
  TARGETDIR = Assets/Scripts/Binaries
  TARGET = $(TARGETDIR)/Sandbox.dll
  OBJDIR = Intermediates/Debug
  FLAGS = /noconfig
  DEPENDS = ../Resources/Scripts/Fox-ScriptCore.dll
  REFERENCES = /r:../Resources/Scripts/Fox-ScriptCore.dll
  define PREBUILDCMDS
  endef
  define PRELINKCMDS
  endef
  define POSTBUILDCMDS
  endef
endif

ifeq ($(config),release)
  CSC = csc
  RESGEN = resgen
  TARGETDIR = Assets/Scripts/Binaries
  TARGET = $(TARGETDIR)/Sandbox.dll
  OBJDIR = Intermediates/Release
  FLAGS = /optimize /noconfig
  DEPENDS = ../Resources/Scripts/Fox-ScriptCore.dll
  REFERENCES = /r:../Resources/Scripts/Fox-ScriptCore.dll
  define PREBUILDCMDS
  endef
  define PRELINKCMDS
  endef
  define POSTBUILDCMDS
  endef
endif

ifeq ($(config),dist)
  CSC = csc
  RESGEN = resgen
  TARGETDIR = Assets/Scripts/Binaries
  TARGET = $(TARGETDIR)/Sandbox.dll
  OBJDIR = Intermediates/Dist
  FLAGS = /noconfig
  DEPENDS = ../Resources/Scripts/Fox-ScriptCore.dll
  REFERENCES = /r:../Resources/Scripts/Fox-ScriptCore.dll
  define PREBUILDCMDS
  endef
  define PRELINKCMDS
  endef
  define POSTBUILDCMDS
  endef
endif

FLAGS += /t:library /lib:/home/samuel/Git\ Repos/FoxEngine/FoxEditor/mono/lib/mono/4.5
REFERENCES += /r:../mono/lib/mono/4.5/System.Collections.Immutable.dll /r:../mono/lib/mono/4.5/System.ComponentModel.Composition.dll /r:../mono/lib/mono/4.5/System.ComponentModel.DataAnnotations.dll /r:../mono/lib/mono/4.5/System.Configuration.dll /r:../mono/lib/mono/4.5/System.Configuration.Install.dll /r:../mono/lib/mono/4.5/System.Core.dll /r:../mono/lib/mono/4.5/System.Data.DataSetExtensions.dll /r:../mono/lib/mono/4.5/System.Data.dll /r:../mono/lib/mono/4.5/System.Data.Entity.dll /r:../mono/lib/mono/4.5/System.Data.Linq.dll /r:../mono/lib/mono/4.5/System.Data.OracleClient.dll /r:../mono/lib/mono/4.5/System.Data.Services.Client.dll /r:../mono/lib/mono/4.5/System.Data.Services.dll /r:../mono/lib/mono/4.5/System.Deployment.dll /r:../mono/lib/mono/4.5/System.Design.dll /r:../mono/lib/mono/4.5/System.DirectoryServices.dll /r:../mono/lib/mono/4.5/System.DirectoryServices.Protocols.dll /r:../mono/lib/mono/4.5/System.dll /r:../mono/lib/mono/4.5/System.Drawing.Design.dll /r:../mono/lib/mono/4.5/System.Drawing.dll /r:../mono/lib/mono/4.5/System.Dynamic.dll /r:../mono/lib/mono/4.5/System.EnterpriseServices.dll /r:../mono/lib/mono/4.5/System.IdentityModel.dll /r:../mono/lib/mono/4.5/System.IdentityModel.Selectors.dll /r:../mono/lib/mono/4.5/System.IO.Compression.dll /r:../mono/lib/mono/4.5/System.IO.Compression.FileSystem.dll /r:../mono/lib/mono/4.5/System.Json.dll /r:../mono/lib/mono/4.5/System.Json.Microsoft.dll /r:../mono/lib/mono/4.5/System.Management.dll /r:../mono/lib/mono/4.5/System.Memory.dll /r:../mono/lib/mono/4.5/System.Messaging.dll /r:../mono/lib/mono/4.5/System.Net.dll /r:../mono/lib/mono/4.5/System.Net.Http.dll /r:../mono/lib/mono/4.5/System.Net.Http.Formatting.dll /r:../mono/lib/mono/4.5/System.Net.Http.WebRequest.dll /r:../mono/lib/mono/4.5/System.Numerics.dll /r:../mono/lib/mono/4.5/System.Numerics.Vectors.dll /r:../mono/lib/mono/4.5/System.Reactive.Core.dll /r:../mono/lib/mono/4.5/System.Reactive.Debugger.dll /r:../mono/lib/mono/4.5/System.Reactive.Experimental.dll /r:../mono/lib/mono/4.5/System.Reactive.Interfaces.dll /r:../mono/lib/mono/4.5/System.Reactive.Linq.dll /r:../mono/lib/mono/4.5/System.Reactive.Observable.Aliases.dll /r:../mono/lib/mono/4.5/System.Reactive.PlatformServices.dll /r:../mono/lib/mono/4.5/System.Reactive.Providers.dll /r:../mono/lib/mono/4.5/System.Reactive.Runtime.Remoting.dll /r:../mono/lib/mono/4.5/System.Reactive.Windows.Forms.dll /r:../mono/lib/mono/4.5/System.Reactive.Windows.Threading.dll /r:../mono/lib/mono/4.5/System.Reflection.Context.dll /r:../mono/lib/mono/4.5/System.Reflection.Metadata.dll /r:../mono/lib/mono/4.5/System.Runtime.Caching.dll /r:../mono/lib/mono/4.5/System.Runtime.CompilerServices.Unsafe.dll /r:../mono/lib/mono/4.5/System.Runtime.DurableInstancing.dll /r:../mono/lib/mono/4.5/System.Runtime.Remoting.dll /r:../mono/lib/mono/4.5/System.Runtime.Serialization.dll /r:../mono/lib/mono/4.5/System.Runtime.Serialization.Formatters.Soap.dll /r:../mono/lib/mono/4.5/System.Security.dll /r:../mono/lib/mono/4.5/System.ServiceModel.Activation.dll /r:../mono/lib/mono/4.5/System.ServiceModel.Discovery.dll /r:../mono/lib/mono/4.5/System.ServiceModel.dll /r:../mono/lib/mono/4.5/System.ServiceModel.Internals.dll /r:../mono/lib/mono/4.5/System.ServiceModel.Routing.dll /r:../mono/lib/mono/4.5/System.ServiceModel.Web.dll /r:../mono/lib/mono/4.5/System.ServiceProcess.dll /r:../mono/lib/mono/4.5/System.Threading.Tasks.Dataflow.dll /r:../mono/lib/mono/4.5/System.Threading.Tasks.Extensions.dll /r:../mono/lib/mono/4.5/System.Transactions.dll /r:../mono/lib/mono/4.5/System.Web.Abstractions.dll /r:../mono/lib/mono/4.5/System.Web.ApplicationServices.dll /r:../mono/lib/mono/4.5/System.Web.dll /r:../mono/lib/mono/4.5/System.Web.DynamicData.dll /r:../mono/lib/mono/4.5/System.Web.Extensions.Design.dll /r:../mono/lib/mono/4.5/System.Web.Extensions.dll /r:../mono/lib/mono/4.5/System.Web.Http.dll /r:../mono/lib/mono/4.5/System.Web.Http.SelfHost.dll /r:../mono/lib/mono/4.5/System.Web.Http.WebHost.dll /r:../mono/lib/mono/4.5/System.Web.Mobile.dll /r:../mono/lib/mono/4.5/System.Web.Mvc.dll /r:../mono/lib/mono/4.5/System.Web.Razor.dll /r:../mono/lib/mono/4.5/System.Web.RegularExpressions.dll /r:../mono/lib/mono/4.5/System.Web.Routing.dll /r:../mono/lib/mono/4.5/System.Web.Services.dll /r:../mono/lib/mono/4.5/System.Web.WebPages.Deployment.dll /r:../mono/lib/mono/4.5/System.Web.WebPages.dll /r:../mono/lib/mono/4.5/System.Web.WebPages.Razor.dll /r:../mono/lib/mono/4.5/System.Windows.dll /r:../mono/lib/mono/4.5/System.Windows.Forms.DataVisualization.dll /r:../mono/lib/mono/4.5/System.Windows.Forms.dll /r:../mono/lib/mono/4.5/System.Workflow.Activities.dll /r:../mono/lib/mono/4.5/System.Workflow.ComponentModel.dll /r:../mono/lib/mono/4.5/System.Workflow.Runtime.dll /r:../mono/lib/mono/4.5/System.Xaml.dll /r:../mono/lib/mono/4.5/System.Xml.dll /r:../mono/lib/mono/4.5/System.Xml.Linq.dll /r:../mono/lib/mono/4.5/System.Xml.Serialization.dll

SOURCES += \
	Assets/Scripts/Source/Bullet.cs \
	Assets/Scripts/Source/CameraFollow.cs \
	Assets/Scripts/Source/Player.cs \

EMBEDFILES += \

RESPONSE += $(OBJDIR)/Sandbox.rsp
SHELLTYPE := posix
ifeq (.exe,$(findstring .exe,$(ComSpec)))
	SHELLTYPE := msdos
endif

all: $(TARGETDIR) $(OBJDIR) prebuild $(EMBEDFILES) $(COPYFILES) prelink $(TARGET)

$(TARGET): $(SOURCES) $(EMBEDFILES) $(DEPENDS) $(RESPONSE)
	$(SILENT) $(CSC) /nologo /out:$@ $(FLAGS) $(REFERENCES) @$(RESPONSE) $(patsubst %,/resource:%,$(EMBEDFILES))
	$(POSTBUILDCMDS)

$(TARGETDIR):
	@echo Creating $(TARGETDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(TARGETDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(TARGETDIR))
endif

$(RESPONSE): Sandbox.make
	@echo Generating response file
ifeq (posix,$(SHELLTYPE))
	$(SILENT) rm -f $(RESPONSE)
else
	$(SILENT) if exist $(RESPONSE) del $(OBJDIR)\Sandbox.rsp
endif
	@echo Assets/Scripts/Source/Bullet.cs >> $(RESPONSE)
	@echo Assets/Scripts/Source/CameraFollow.cs >> $(RESPONSE)
	@echo Assets/Scripts/Source/Player.cs >> $(RESPONSE)

$(OBJDIR):
	@echo Creating $(OBJDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(OBJDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(OBJDIR))
endif

prebuild:
	$(PREBUILDCMDS)

prelink:
	$(PRELINKCMDS)