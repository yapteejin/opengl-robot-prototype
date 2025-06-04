# OpenGL Robot Prototype

This is a **robot prototype** created as part of a **Year 2 Graphics Programming Assignment**. 

The application uses **OpenGL and C++ (via Visual Studio)** to render and animate a 3D robot model with support for camera controls, lighting, object transformations, and interactive keyboard input.

---

## Getting Started

### Requirements
- Visual Studio 2019 or newer
- Windows OS
- OpenGL-compatible graphics card

### How to Build
1. Clone or download the repository.
2. Open the `.sln` file in Visual Studio.
3. Build the solution (`Ctrl + Shift + B`).
4. Run the program (`F5`).

---

## Troubleshooting

### 1. Linker Errors

Make sure the following library is linked:
- `opengl32.lib`

#### How to Add Them
1. Project → Properties
2. `Linker → Input → Additional Dependencies`
3. Add: `OpenGL32.lib`

it should look like this `OpenGL32.lib;%(AdditionalDependencies)`



### 2. Entry Point Error: `unresolved external symbol main`

#### Fix:
- Project → Properties
- Go to `Linker → System → Subsystem`
- Set it to: `Not Set`



### 3. String Type Error: Cannot convert `const char*` to `LPCWSTR`

#### Fix:
- Project → Properties
- Go to `Advanced → Character Set`
- Set it to: `Use Multi-Byte Character Set`

---

## Keyboard Controls

### Mode Toggles
| Key | Description                                                                                                                |
| --- | -------------------------------------------------------------------------------------------------------------------------- |
| `0` | Switch between rotation mode or manipulating the center for `gluLookAt` in perspective view (**Rotation mode by default**) |
| `6` | Toggle individual part movement                                                                                            |
| `7` | Pause/resume light source rotation (**On by default**)                                                                     |
| `8` | Toggle light source on/off (**On by default**)                                                                             |
| `P` | Switch between orthographic and perspective view (**Orthographic by default**)                                             |

### Object Transformation
| Key        | Description                          |
| ---------- | ------------------------------------ |
| `A`, `D`   | Rotate model horizontally            |
| `W`, `S`   | Rotate model vertically              |
| `Q`, `E`   | Rotate model on z-axis               |
| `SPACEBAR` | Reset all transformations and camera |

### Camera Control
> Note: The camera is in orthographic view by default.

| Key                          | Description                                                                             |
| ---------------------------- | --------------------------------------------------------------------------------------- |
| `J`, `L`                     | Move reference point (orthographic) / Move camera (perspective)                         |
| `I`, `K`                     | Move reference point vertically (orthographic) / camera vertically (perspective)        |
| `U`, `O`                     | Move camera on z-axis (perspective)                                                     |
| `A`, `D`, `W`, `S`, `Q`, `E` | Move reference point in perspective view (Press `0` to enable center manipulation mode) |

### Robot Movement
> Requires jetpack to be turned on to work. 
> Press T to enable jetpack; R to disable jetpack


| Key                  | Description                            |
| -------------------- | -------------------------------------- |
| Arrow Keys           | Move robot horizontally and vertically |
| `NUMPAD7`, `NUMPAD9` | Move robot on z-axis                   |

### Individual Part Control
> Press `6` first to enter part movement mode

#### Rotation
| Key                  | Description                       |
| -------------------- | --------------------------------- |
| `NUMPAD1`, `NUMPAD3` | Rotate selected part horizontally |
| `NUMPAD2`, `NUMPAD5` | Rotate selected part vertically   |
| `NUMPAD4`, `NUMPAD6` | Rotate selected part on z-axis    |

#### Part Selection
| Key | Body Part       |
| --- | --------------- |
| `M` | Lower left leg  |
| `N` | Lower right leg |
| `B` | Whole left leg  |
| `V` | Whole right leg |
| `C` | Head            |
| `X` | Whole left arm  |
| `Z` | Whole right arm |
| `H` | Lower left arm  |
| `G` | Lower right arm |


### Robot Actions
| Key | Description                       |
| --- | --------------------------------- |
| `F` | Fire weapon (Hold for rapid fire) |
| `T` | Activate jetpack                  |
| `R` | Deactivate jetpack                |

### Predefined Poses
| Key | Description                               |
| --- | ----------------------------------------- |
| `1` | Idle pose                                 |
| `2` | Flying with shield up                     |
| `3` | Gun pose                                  |
| `4` | Victory pose                              |
| `5` | Running animation                         |
| `6` | Manual part movement (see toggle section) |

  
