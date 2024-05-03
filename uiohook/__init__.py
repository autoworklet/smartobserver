from torch.utils.cpp_extension import load
import sys
import os

dirname = os.path.dirname(__file__)


# Determine the platform-specific source files and directories
uiohook_sources = [
    # os.path.join(dirname, "system.hpp"),
    os.path.join(dirname, "main.cpp"),
    os.path.join(dirname, "uiohook_worker.c"),
    os.path.join(dirname, "src", "logger.c"),
]
include_dirs = [
    dirname,
    os.path.join(dirname, "src"),
]
library_dirs = [
]
libraries = [
    
]  # The name of the libUIOHook library

# Platform-specific adjustments
if sys.platform.startswith("win"):
    # Windows specific settings
    uiohook_sources += [
        os.path.join(dirname, "src", "windows", "input_helper.c"),
        os.path.join(dirname, "src", "windows", "input_hook.c"),
        os.path.join(dirname, "src", "windows", "post_event.c"),
        os.path.join(dirname, "src", "windows", "system_properties.c"),
    ]
elif sys.platform.startswith("darwin"):
    # macOS specific settings
    uiohook_sources += [
        os.path.join(dirname, "src", "darwin", "input_helper.c"),
        os.path.join(dirname, "src", "darwin", "input_hook.c"),
        os.path.join(dirname, "src", "darwin", "post_event.c"),
        os.path.join(dirname, "src", "darwin", "system_properties.c"),
    ]
    libraries += ["-framework", "ApplicationServices"]
else:
    # Assuming Linux/X11 for simplicity
    uiohook_sources += [
        os.path.join(dirname, "src", "x11", "input_helper.c"),
        os.path.join(dirname, "src", "x11", "input_hook.c"),
        os.path.join(dirname, "src", "x11", "post_event.c"),
        os.path.join(dirname, "src", "x11", "system_properties.c"),
    ]
    libraries += ["-lX11", "-lxtst"]  # Example of adding additional libraries

from typing import Callable, Dict
class IOHook:
    @staticmethod
    def start(callback: Callable[[Dict], None]) -> None:
        """
        Start the event loop to listen for input events.

        Args:
            callback: A function to be called when an event occurs. This function takes a single argument,
                      a dictionary, that contains details about the event.
        """
        pass

    @staticmethod
    def stop() -> None:
        """
        Stop the event loop.
        """
        pass

# Load the extension
pyiohook: IOHook = load(
    name="pyiohook",
    sources=uiohook_sources,
    extra_include_paths=include_dirs,
    extra_ldflags=libraries,
    extra_cflags=[],
    with_cuda=False,
    verbose=True
)
