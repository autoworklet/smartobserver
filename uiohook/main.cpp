#include <torch/extension.h>

#include <iostream>

#include "uiohook_worker.h"

#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)
#define PYBIND11_DETAILED_ERROR_MESSAGES

namespace py = pybind11;

static std::function<void(py::dict)>* global_callback = nullptr;
static void dispatcher(uiohook_event *const event) {
    if (global_callback != nullptr) {
        py::gil_scoped_acquire acquire; // Acquire GIL
        py::dict event_dict;

        // Fill event_dict with the necessary fields
        event_dict["type"] = static_cast<int>(event->type); // Cast to int if necessary
        // typedef enum _event_type {
        //     EVENT_HOOK_ENABLED = 1,
        //     EVENT_HOOK_DISABLED,
        //     EVENT_KEY_TYPED,
        //     EVENT_KEY_PRESSED,
        //     EVENT_KEY_RELEASED,
        //     EVENT_MOUSE_CLICKED,
        //     EVENT_MOUSE_PRESSED,
        //     EVENT_MOUSE_RELEASED,
        //     EVENT_MOUSE_MOVED,
        //     EVENT_MOUSE_DRAGGED,
        //     EVENT_MOUSE_WHEEL
        // } event_type;
        // convert event_dict["type"] to string
        if (event->type == EVENT_HOOK_ENABLED) {
          event_dict["type"] = "EVENT_HOOK_ENABLED";
        } else if (event->type == EVENT_HOOK_DISABLED) {
          event_dict["type"] = "EVENT_HOOK_DISABLED";
        } else if (event->type == EVENT_KEY_TYPED) {
          event_dict["type"] = "EVENT_KEY_TYPED";
        } else if (event->type == EVENT_KEY_PRESSED) {
          event_dict["type"] = "EVENT_KEY_PRESSED";
        } else if (event->type == EVENT_KEY_RELEASED) {
          event_dict["type"] = "EVENT_KEY_RELEASED";
        } else if (event->type == EVENT_MOUSE_CLICKED) {
          event_dict["type"] = "EVENT_MOUSE_CLICKED";
        } else if (event->type == EVENT_MOUSE_PRESSED) {
          event_dict["type"] = "EVENT_MOUSE_PRESSED";
        } else if (event->type == EVENT_MOUSE_RELEASED) {
          event_dict["type"] = "EVENT_MOUSE_RELEASED";
        } else if (event->type == EVENT_MOUSE_MOVED) {
          event_dict["type"] = "EVENT_MOUSE_MOVED";
        } else if (event->type == EVENT_MOUSE_DRAGGED) {
          event_dict["type"] = "EVENT_MOUSE_DRAGGED";
        } else if (event->type == EVENT_MOUSE_WHEEL) {
          event_dict["type"] = "EVENT_MOUSE_WHEEL";
        }
        event_dict["time"] = event->time;
        event_dict["mask"] = (int)(event->mask);
        // change mask to flags string "alt,control,shift,meta,fn"
        // flags = [];
        std::vector<std::string> flags;
        if ((int)(event->mask) & (int)(MASK_ALT)) {
          // std vector append
          flags.push_back("alt");
        }
        if ((int)(event->mask) & (int)(MASK_CTRL)) {
          flags.push_back("control");
        }
        if ((int)(event->mask) & (int)(MASK_SHIFT)) {
          flags.push_back("shift");
        }
        if ((int)(event->mask) & (int)(MASK_META)) {
          flags.push_back("cmd");
        }
        // if (event->mask & MOD_FN) { // not supported
        //   flags.push_back("fn");
        // }        
        // Join the flags with commas
        std::string joined_flags;
        for (const auto& flag : flags) {
            joined_flags += flag + ",";
        }
        // Remove trailing comma
        if (!joined_flags.empty()) {
            joined_flags.pop_back();
        }
        event_dict["flags"] = py::str(joined_flags);

        event_dict["reserved"] = event->reserved;
        
        // Handling the union data. This example generically converts it to a Python dict.
        /* condition to check if event is a keyboard event */
        if (
          event->type == EVENT_KEY_PRESSED || 
          event->type == EVENT_KEY_RELEASED ||
          event->type == EVENT_KEY_TYPED
        ) {
          event_dict["keycode"] = event->data.keyboard.keycode;
          event_dict["keychar"] = event->data.keyboard.keychar;
          event_dict["rawcode"] = event->data.keyboard.rawcode;
        } else if (
          event->type == EVENT_MOUSE_PRESSED || event->type == EVENT_MOUSE_RELEASED ||
          event->type == EVENT_MOUSE_CLICKED || event->type == EVENT_MOUSE_MOVED ||
          event->type == EVENT_MOUSE_DRAGGED
        ) {
          event_dict["button"] = event->data.mouse.button;
          // change button to string
          if (event->data.mouse.button == 1) {
            event_dict["button"] = "left";
          } else if (event->data.mouse.button == 2) {
            event_dict["button"] = "right";
          } else if (event->data.mouse.button == 3) {
            event_dict["button"] = "middle";
          } else if (event->data.mouse.button == 4) {
            event_dict["button"] = "wheel_up";
          } else if (event->data.mouse.button == 5) {
            event_dict["button"] = "wheel_down";
          }
          event_dict["x"] = event->data.mouse.x;
          event_dict["y"] = event->data.mouse.y;
        } else if (event->type == EVENT_MOUSE_WHEEL) {
          event_dict["x"] = event->data.wheel.x;
          event_dict["y"] = event->data.wheel.y;
          event_dict["wheelType"] = event->data.wheel.type;
          event_dict["direction"] = event->data.wheel.direction;
          event_dict["amount"] = event->data.wheel.amount;
          event_dict["rotation"] = event->data.wheel.rotation;
        }
        
        // Call the stored callback
        (*global_callback)(event_dict);
    }
}

PYBIND11_MODULE(pyiohook, m) {
  m.def("start", [](const py::function& callback) {
      // Store the Python callback in the global variable
      static std::function<void(py::dict)> callback_wrapper = [callback](py::dict event_dict) mutable {
          callback(event_dict);
      };
      global_callback = &callback_wrapper;

      int worker_status = uiohook_worker_start(dispatcher); // Use the static dispatcher
      if (worker_status != UIOHOOK_SUCCESS) {
          std::cout << "Failed to start worker thread" << std::endl;
      }
  },
  R"pbdoc(
      Start the event loop
  )pbdoc");

  m.def("stop", []() {
      uiohook_worker_stop();
      global_callback = nullptr;
  },
  R"pbdoc(
      Stop the event loop
  )pbdoc");

#ifdef VERSION_INFO
  m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
  m.attr("__version__") = "dev";
#endif
}