# TaskGuard
## A Simple and Secure To-Do List Application

TaskGuard is a to-do list application that helps you manage your tasks in a simple and efficient way. What sets TaskGuard apart is its focus on security, with robust encryption mechanisms to protect your data.

![image](https://user-images.githubusercontent.com/123785508/234422735-0a73b366-f11b-410c-828f-7b2de99d8683.png)

**To encrypt the database, we use the [QtCipherSqlitePlugin](https://github.com/mxrcode/QtCipherSqlitePlugin).*
  
## Features
- **Task management**: Create, edit, and delete tasks with ease.
- Encryption: TaskGuard uses **ChaCha20-Poly1305** encryption to secure your task data, ensuring that your information remains confidential.
- Password encryption: TaskGuard encrypts your **password** using **AES-256-CBC**, ensuring that it is securely stored in the system.
- **Hardware ID (HWID) encryption**: TaskGuard provides an additional layer of security by allowing you to encrypt the database using your computer's HWID. This makes the database file useless on  another computer, even if an intruder has the password.
- **Cross-platform**: TaskGuard is currently written and compiled for Windows using C++ and Qt 6.4.3, but it can be compiled for other systems as well.
- Already available in **4 languages**: English, Ukrainian, Russian, Spanish.

## Installation
*Dependencies must be installed in the system for the program to run and work correctly.*

- Download the latest version of TaskGuard https://github.com/mxrcode/TaskGuard/releases
- Unpack the **.zip** archive to any place convenient for you
- Run the app with **TaskGuard.exe**

*An icon should appear in the tray by clicking on which a menu for interacting with the widget will appear. There you can configure it and add the program to autorun.*

**Dependencies:** [vcredist_x64(2015-2022)](https://aka.ms/vs/17/release/vc_redist.x64.exe) [(x86)](https://aka.ms/vs/17/release/vc_redist.x86.exe) 

## License
TaskGuard is released under the GNU Lesser General Public License (LGPL-3.0), which means it is free and open-source software that you can use, modify, and distribute. Please refer to the [LICENSE](https://github.com/mxrcode/TaskGuard/blob/main/LICENSE) file for more details.

## Contributing
If you would like to contribute to TaskGuard, you are welcome to submit pull requests or report issues on the GitHub repository. Contributions are greatly appreciated and help improve the application.

## Security
TaskGuard takes data security seriously and implements encryption mechanisms to protect your information. However, no software is entirely secure, and it is important to use TaskGuard and any other software with caution. Please report any security concerns or vulnerabilities to the project maintainers.

## Disclaimer
TaskGuard is provided "as is" without any warranty or guarantee of any kind. The developers of TaskGuard are not responsible for any loss or damage caused by the use of this application.

## Contact
If you have any questions, comments, or feedback, please feel free to contact the project maintainers via the GitHub repository.

Thank you for using TaskGuard! We hope it helps you stay organized and secure.

**Important Note: TaskGuard is a Working Title**

Please note that "TaskGuard" & "Retask" is currently a working title for the to-do list application. As the project evolves, the name may change in the future. Therefore, it's recommended to subscribe to updates or follow the GitHub repository to stay informed about any important changes, including potential updates to the project name.

We appreciate your interest and support, and we are committed to continuously improving and enhancing the application. Thank you for your understanding, and we look forward to bringing you the best possible task management solution!
