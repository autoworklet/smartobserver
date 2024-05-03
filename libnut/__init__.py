from .cpp_extension import load
import sys
import os

dirname = os.path.join(os.path.dirname(__file__))
src_dirname = os.path.join(dirname, "src")

# Determine the platform-specific source files and directories
sources = [
    os.path.join(dirname, 'src', "deadbeef_rand.c"),
    os.path.join(dirname, 'src', "MMBitmap.c"),
    os.path.join(dirname, "main.cpp"),
]
include_dirs = [
    dirname,
    src_dirname,
]
library_dirs = [
]
libraries = [

]  # The name of the libUIOHook library

# Platform-specific adjustments
if sys.platform.startswith("win"):
    # Windows specific settings
    # highlightwindow.c keycode.c         keypress.c        mouse.c           screen.c          screengrab.c      window_manager.cc
    sources += [
        os.path.join(src_dirname, "win32", "highlightwindow.c"),
        os.path.join(src_dirname, "win32", "keycode.c"),
        os.path.join(src_dirname, "win32", "keypress.c"),
        os.path.join(src_dirname, "win32", "mouse.c"),
        os.path.join(src_dirname, "win32", "screen.c"),
        os.path.join(src_dirname, "win32", "screengrab.c"),
        os.path.join(src_dirname, "win32", "window_manager.cc"),
    ]
    libraries += [
        "-L"+os.path.join(src_dirname, "3rdparty", "win32"),
    ]
elif sys.platform.startswith("darwin"):
    # macOS specific settings
    # highlightwindow.m keycode.c         keypress.c        mouse.c           mouse_utils.mm    screen.c          screengrab.m      window_manager.mm
    sources += [
        os.path.join(src_dirname, "macos", "highlightwindow.m"),
        os.path.join(src_dirname, "macos", "keycode.c"),
        os.path.join(src_dirname, "macos", "keypress.c"),
        os.path.join(src_dirname, "macos", "mouse.c"),
        os.path.join(src_dirname, "macos", "mouse_utils.mm"),
        os.path.join(src_dirname, "macos", "screen.c"),
        os.path.join(src_dirname, "macos", "screengrab.m"),
        os.path.join(src_dirname, "macos", "window_manager.mm"),
    ]
    include_dirs += [
        os.path.join(src_dirname, "macos"),
    ]
    libraries += ["-framework", "ApplicationServices", "-framework", "Cocoa"]
else:
    # Assuming Linux/X11 for simplicity
    # highlightwindow.c keycode.c         keypress.c        mouse.c           screen.c          screengrab.c      window_manager.cc xdisplay.c
    sources += [
        os.path.join(src_dirname, "linux", "highlightwindow.c"),
        os.path.join(src_dirname, "linux", "keycode.c"),
        os.path.join(src_dirname, "linux", "keypress.c"),
        os.path.join(src_dirname, "linux", "mouse.c"),
        os.path.join(src_dirname, "linux", "screen.c"),
        os.path.join(src_dirname, "linux", "screengrab.c"),
        os.path.join(src_dirname, "linux", "window_manager.cc"),
        os.path.join(src_dirname, "linux", "xdisplay.c"),
    ]
    libraries += ["-lX11", "-lxtst"]

# Nut type definition
from typing import Tuple, List, Dict, Any

class MMPoint:
    def __init__(self, x: int, y: int):
        self.x: int = x
        self.y: int = y

class MMSize:
    def __init__(self, width: int, height: int):
        self.width: int = width
        self.height: int = height

class MMRect:
    def __init__(self, origin: MMPoint, size: MMSize):
        self.origin: MMPoint = origin
        self.size: MMSize = size

class Nut:
    @staticmethod
    def dragMouse(x: int, y: int, button: str = "left") -> int: ...
    @staticmethod
    def moveMouse(x: int, y: int) -> int: ...
    @staticmethod
    def getMousePos() -> Dict[str, int]: ...
    @staticmethod
    def mouseClick(button: str = "left", double_click: bool = False) -> int: ...
    @staticmethod
    def mouseToggle(down: bool, button: str = "left") -> int: ...
    @staticmethod
    def setMouseDelay(delay: int) -> int: ...
    @staticmethod
    def scrollMouse(x: int, y: int) -> int: ...
    
    @staticmethod
    def keyTap(key_name: str, flags: str = "none") -> int: ...
    @staticmethod
    def keyToggle(key_name: str, down: bool, flags: str = "none") -> int: ...
    @staticmethod
    def keyToggleRaw(key_code: int, down: bool, flags: str = "none") -> int: ...
    @staticmethod
    def typeString(string: str) -> int: ...
    @staticmethod
    def typeStringDelayed(string: str, cpm: int) -> int: ...
    @staticmethod
    def setKeyboardDelay(delay: int) -> int: ...
    
    @staticmethod
    def getScreenSize() -> Dict[str, int]: ...
    @staticmethod
    def getXDisplayName() -> str: ...
    @staticmethod
    def setXDisplayName(display_name: str) -> int: ...
    @staticmethod
    def highlight(x: int, y: int, width: int, height: int, duration: float, opacity: float) -> int: ...
    @staticmethod
    def getActiveWindow() -> int: ...
    @staticmethod
    def getWindows() -> List[int]: ...
    @staticmethod
    def getWindowRect(window_handle: int) -> Dict[str, int]: ...
    @staticmethod
    def getWindowTitle(window_handle: int) -> str: ...
    @staticmethod
    def focusWindow(window_handle: int) -> bool: ...
    @staticmethod
    def resizeWindow(window_handle: int, width: int, height: int) -> bool: ...
    @staticmethod
    def moveWindow(window_handle: int, x: int, y: int) -> bool: ...
    @staticmethod
    def captureScreen(x: int = 0, y: int = 0, width: int = 0, height: int = 0) -> Dict[str, Any]: ...
    @staticmethod
    def captureScreenToFile(filename: str, x: int = 0, y: int = 0, width: int = 0, height: int = 0) -> int: ...

    # m.def("microsleep", &microsleep, py::arg("microseconds"));
    @staticmethod
    def microsleep(microseconds: float) -> int: ...

# Load the extension
pylibnut: Nut = load(
    name="pylibnut",
    sources=sources,
    extra_include_paths=include_dirs,
    extra_ldflags=libraries,
    extra_cflags=['-std=c++17'],
    with_cuda=False,
    verbose=True
)