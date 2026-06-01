# EasyNote Build & Packaging Guide

## 自动化打包说明

本项目配置了 GitHub Actions 自动化工作流，可以自动构建和打包 Windows 和 Linux Debian 包。

### 工作流说明

#### 1. **自动发布工作流** (`build-release.yml`)
- **触发条件**：推送带 `v*.*.*` 格式的标签 (例如 `v1.0.0`)
- **执行内容**：
  - 在 Windows 上编译并打包 (Qt5)
  - 在 Debian 10 上编译并生成 DEB 包 (Qt5)
  - 自动创建 GitHub Release 并上传打包文件

**使用方法**：
```bash
git tag v1.0.0
git push origin v1.0.0
```

#### 2. **手动测试工作流** (`build-manual.yml`)
- **触发条件**：通过 GitHub 网页手动触发
- **执行内容**：可选择编译 Windows 或 Linux（或两者）
- **用途**：测试新代码或快速构建

**使用方法**：
1. 打开 GitHub 网页：https://github.com/magicdmer/EasyNote/actions
2. 进入 "Manual Build Test"
3. 点击 "Run workflow" 选择要编译的平台

---

## DEB 包结构说明

自动生成的 DEB 包采用以下结构：

```
EasyNote-{version}-amd64.deb
├── DEBIAN/
│   ├── control          # 包元数据（依赖、版本等）
│   └── postinst          # 安装后脚本（更新桌面数据库）
├── opt/
│   └── EasyNote/
│       └── EasyNote      # 应用二进制文件
├── usr/
│   ├── bin/
│   │   └── EasyNote      # 符号链接指向 /opt/EasyNote/EasyNote
│   └── share/
│       ├── applications/
│       │   └── EasyNote.desktop  # 桌面文件
│       └── icons/
│           └── hicolor/64x64/apps/
│               └── EasyNote.png  # 应用图标
```

### DEB 包依赖

```
Depends: 
  libc6 (>= 2.28)           # C 标准库
  libgl1                    # OpenGL 支持
  libqt5core5a (>= 5.11.3)  # Qt5 Core
  libqt5gui5 (>= 5.11.3)    # Qt5 GUI
  libqt5network5 (>= 5.11.3) # Qt5 网络
  libqt5widgets5 (>= 5.11.3) # Qt5 控件
  libstdc++6                # C++ 标准库
  libx11-6                  # X11 库
```

---

## 安装使用

### 从 Release 下载安装

1. **下载 DEB 包**：
   ```bash
   wget https://github.com/magicdmer/EasyNote/releases/download/v1.0.0/EasyNote-1.0.0-amd64.deb
   ```

2. **安装**：
   ```bash
   # 方法1：使用 apt
   sudo apt install ./EasyNote-1.0.0-amd64.deb
   
   # 方法2：使用 dpkg
   sudo dpkg -i EasyNote-1.0.0-amd64.deb
   sudo apt install -f  # 自动安装依赖
   ```

3. **运行**：
   ```bash
   EasyNote
   # 或从应用菜单启动
   ```

4. **卸载**：
   ```bash
   sudo apt remove easynote
   ```

---

## 本地开发环境配置

### Windows (Qt5)

**安装 Qt5**：
1. 下载 Qt5.15.11：https://www.qt.io/download
2. 选择 MSVC 2019 64-bit 版本
3. 安装到默认位置

**编译**：
```bash
qmake EasyNote.pro
nmake release
```

**依赖部署**：
```bash
windeployqt.exe release/EasyNote.exe
```

### Linux - Debian 10 (Qt5)

**安装依赖**：
```bash
sudo apt-get update
sudo apt-get install -y \
  qt5-qmake \
  qtbase5-dev \
  libqt5core5a \
  libqt5gui5 \
  libqt5widgets5 \
  libqt5network5 \
  build-essential \
  libgl1-mesa-dev \
  dpkg-dev
```

**编译**：
```bash
qmake EasyNote.pro
make -j$(nproc)
```

**打包成 DEB**：
```bash
# 创建 DEB 目录结构
mkdir -p deb_root/DEBIAN
mkdir -p deb_root/opt/EasyNote
mkdir -p deb_root/usr/bin
mkdir -p deb_root/usr/share/applications

# 复制二进制文件
cp EasyNote deb_root/opt/EasyNote/
chmod 755 deb_root/opt/EasyNote/EasyNote

# 创建符号链接
ln -s /opt/EasyNote/EasyNote deb_root/usr/bin/EasyNote

# 创建 control 文件
cat > deb_root/DEBIAN/control << EOF
Package: easynote
Version: 1.0.0
Section: utils
Priority: optional
Architecture: amd64
Homepage: https://github.com/magicdmer/EasyNote
Depends: libc6 (>= 2.28), libgl1, libqt5core5a (>= 5.11.3), libqt5gui5 (>= 5.11.3), libqt5network5 (>= 5.11.3), libqt5widgets5 (>= 5.11.3), libstdc++6, libx11-6
Maintainer: magicdmer <magicdmer@gmail.com>
Installed-Size: 5120
Description: A simple and useful note app
 EasyNote is a lightweight note-taking application based on Qt framework.
EOF

# 打包
dpkg-deb --build deb_root EasyNote-1.0.0-amd64.deb
```

---

## 兼容性说明

### 为什么选择 Debian 10？

**向下兼容策略**：
- ✅ 在 Debian 10 上编译 → 可运行在 Debian 10+ 和 Ubuntu 20.04+ 
- ✅ 在低版本编译 → 获得最大兼容性
- ✅ DEB 包采用最小 glibc 版本（Debian 10 默认 2.28）

**支持范围**：
| 操作系统 | 版本 | 兼容性 |
|---------|------|-------|
| Debian | 10+ | ✅ |
| Ubuntu | 20.04 LTS+ | ✅ |
| Linux Mint | 20+ | ✅ |
| Elementary OS | 5.1+ | ✅ |

---

## 故障排除

### DEB 包安装失败

**问题**：`Depends: libqt5widgets5 but it is not installable`

**解决方案**：
```bash
# 更新包列表
sudo apt update

# 安装依赖
sudo apt install libqt5widgets5

# 再次安装 EasyNote
sudo apt install ./EasyNote-*.deb
```

### 运行时缺少库

**问题**：运行应用时报错 `cannot open shared object file`

**解决方案**：
```bash
# 查看应用的依赖
ldd /opt/EasyNote/EasyNote

# 安装缺失的库
sudo apt install <library-name>
```

### 桌面菜单中找不到应用

**解决方案**：
```bash
# 手动更新桌面数据库
sudo update-desktop-database /usr/share/applications/

# 更新图标缓存
sudo gtk-update-icon-cache /usr/share/icons/hicolor/
```

---

## 打包文件说明

### Windows
- **文件名**：`EasyNote-Windows-Qt5.zip`
- **内容**：可独立运行的应用及其所有 Qt5 依赖 DLL
- **使用**：解压即可运行
- **系统要求**：Windows 7+

### Linux
- **文件名**：`EasyNote-{version}-amd64.deb`
- **内容**：Debian 包，使用 Debian 10 编译确保最大兼容性
- **使用**：`sudo apt install ./EasyNote-*.deb`
- **系统要求**：Debian 10+, Ubuntu 20.04+

---

## 环境要求

| 项目 | Windows | Linux (Debian 10) |
|------|---------|-------------------|
| Qt | 5.15.11 | 5.9.5+ |
| 编译器 | MSVC 2019+ | GCC 6.3+ |
| glibc | - | 2.28 |
| 打包工具 | windeployqt, 7z | dpkg-deb |

---

## 发布流程

1. **准备代码**：完成所有测试和 bug 修复
2. **更新版本号**：在适当的地方更新版本（如果有版本文件）
3. **创建标签**：
   ```bash
   git tag -a v1.0.0 -m "Release version 1.0.0"
   git push origin v1.0.0
   ```
4. **自动构建**：GitHub Actions 自动开始构建
5. **检查 Release**：https://github.com/magicdmer/EasyNote/releases
   - 验证生成的 Windows ZIP 和 Linux DEB 文件
   - 如需修改发布说明，直接在 Release 页编辑

---

## 常见问题

**Q: DEB 包和 AppImage 有什么区别？**
A: 
- DEB 包：标准的 Debian/Ubuntu 包，通过包管理器安装，有完整的卸载和依赖管理。
- AppImage：自包含的可执行文件，跨发行版通用，但需要手动管理依赖。

本项目使用 DEB 包提供更好的系统集成。

**Q: 可以生成 32-bit 版本吗？**
A: 可以，修改工作流中 `Architecture: amd64` 为其他值（如 `i386`），需要确保编译环境支持。

**Q: 如何为不同的 Ubuntu 版本生成不同的包？**
A: 可以在工作流中使用不同的容器镜像（如 `ubuntu:20.04`, `ubuntu:22.04`）并分别生成包。

**Q: DEB 包中可以包含图标文件吗？**
A: 可以。工作流已预留 `usr/share/icons/hicolor/64x64/apps/` 目录，将 PNG 图标放入即可。
