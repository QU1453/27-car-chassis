# car_dipan 修改记录（电赛小车底盘 / STM32G473VET6）

## 2026-10-18　屏幕驱动（Hardware 层）+ 显示“我是G473”

**目的**：加入 ST7789 2.00 寸（320×240）屏驱动，上电显示一行字。驱动独立放 Hardware 层，不改 CubeMX 生成逻辑。

**新增文件**（`Hardware/`）
| 文件 | 作用 |
|---|---|
| lcd_port.h / .c | 引脚 + SPI2 搬运层，直接用 CubeMX 的 `hspi2` |
| lcd_font.h / .c | 8×16 ASCII（95 字，取自厂家例程）+ 16×16 汉字（我、是） |
| lcd.h / .c | ST7789 初始化、清屏、画点/线/框、ASCII/汉字/UTF-8 字符串 |

**接线（由板上走线决定：原理图 STM32G473VET6.pdf + 厂家例程 .ioc 一致）**

| 信号 | 引脚 | 板上网络 | 说明 |
|---|---|---|---|
| RST | PA5 | EXT_SPI1_SCK | GPIO 输出，低复位 |
| BLK | PA6 | EXT_SPI1_MISO | GPIO 输出，高点亮 |
| CS | PA7 | EXT_SPI1_MOSI | GPIO 输出，常低 |
| DC | PB14 | EXT_SPI2_MISO | GPIO 输出 |
| SDA | PB15 | EXT_SPI2_MOSI | SPI2_MOSI（AF5） |
| SCK | **PB13** | EXT_SPI2_SCK | SPI2_SCK（AF5） |

**接口形式（查证结论）**：屏上没有排针，只有一根 16 pin、0.5 mm 间距的 FFC 软排线，
插到板上的 `FPC1 = FPC-05F-16PH20` 座子（座子 17/18 脚是屏蔽/接地脚，不算信号）。
排线里走的就是：`SCK / SDA / DC / CS / RST / BLK / TE` + `VDD / GND / LEDK`，即 4 线单向 SPI。
屏那一侧是 22 pin 的 `FPC2 = AFC07-S22ECA-00`，中间经转接板接到 16 pin 座子。
所以引脚 100% 由 PCB 铜箔决定、不可自选，也不能接开发板排针。

注：PB14 被当 DC 用，所以 SPI2 必须配成 Half-Duplex Master(1LINE)。
为腾出 PB13，CAN 只保留 FDCAN1（PA11/PA12，单个总线带 2 个 C610 足够），删除 FDCAN2；PB12 变为空闲。
（16 pin 排线的脚号 1→16 顺序在 PDF 里画了两列重复数字，无法唯一判读；但信号集合与 MCU 侧映射已由原理图网络名 + 厂家 .ioc 双向确认。）

**CubeMX 要配的东西**
1. SPI2：Mode = Half-Duplex Master（1LINE），Hardware NSS = Disable（软件 NSS，CS 用 GPIO）
2. 参数：Motorola / 8bit / MSB First / Prescaler 16 / CPOL High / CPHA 2 Edge（≈10.6MHz）/ TI Mode Disable / CRC Disable
3. 引脚：PB13 → SPI2_SCK，PB15 → SPI2_MOSI（都设 Pull-up + High speed；SPI1 不启用，PA5/6/7 只作普通 GPIO）
4. GPIO 输出：PA5、PA6、PA7、PB14（推挽、无上下拉、初始低），User Label 依次 `LCD_RST` `LCD_BLK` `LCD_CS` `LCD_DC`
5. FDCAN2 取消勾选（释放 PB12/PB13）
6. Generate Code（Keep User Code 已勾选，勿取消）

**生成结果核对（2026-10-18，CubeMX 6.17 / FW_G4 1.6.3）**
- SPI2 全部参数与厂家例程一致：Master / 1LINE / 8bit / MSB / CPOL High / CPHA 2Edge / 分频 16 = 10.625 MBit/s，NSS 生成为 `SPI_NSS_SOFT`。
- PA5/PA6/PA7/PB14 = GPIO_OUTPUT_PP（初始低），PB13/PB15 = AF5_SPI2，FDCAN2 已移除。
- `Hardware` 分组与 `../Hardware` 头文件路径**重新生成后仍在**，无需重加。
- Keil 编译 0 Error / 0 Warning，Code=9006 RO=2194 RW=16 ZI=2520。
- 两点遗留（不影响先跑）：① PB13/PB15 生成成 `GPIO_NOPULL` + `SPEED_FREQ_LOW`，厂家例程是 PULLUP + HIGH，10.6MHz 下裕量偏小；② 四个 LCD 脚没打 User Label，`main.h` 无 `LCD_*_Pin` 宏，靠 `lcd_port.h` 的 `#ifndef` 兜底，数值一致。
- **注意**：本次把 FDCAN1 也一起删了，当前工程没有任何 CAN。后面接 M2006/C610 前需重新勾 FDCAN1（PA11/PA12，与 LCD 引脚不冲突）。→ **已重新勾选**，生成了 `Core/Src/fdcan.c`、`Core/Inc/fdcan.h`。
- **已修**：链接报 105 条 `L6406E`（可用存储区全为 0）。本工程有**两条互不相干的构建路径**，要分别填内存区：
  - Keil uVision 走 `MDK-ARM/car_dipan.uvprojx`：`IROM=0x8000000/0x80000`、`IRAM=0x20000000/0x20000`、`OCR_RVCT4`=ROM 值、`OCR_RVCT9`=RAM 值、`Ir2Chk=0`（RVCT4/RVCT9 不能写反）。
  - **EIDE 插件**（Trae 里点 Build 走这条）走 `MDK-ARM/.eide/eide.yml`，**完全不读 uvprojx**：其 `storageLayout` 的 IROM1/IRAM1 原是空串、还多勾了一个空 IROM2，于是生成的 `build/car_dipan/car_dipan.sct` 里全是 `0x0/0x0`。已填 `0x8000000/0x80000`、`0x20000000/0x20000`，IROM2 取消勾选。
  - 两条路径均 0 Error / 0 Warning（Code=9006）。

**代码改动**
- 新增 `Hardware/lcd_port.*`、`lcd_font.*`、`lcd.*`
- `Core/Src/main.c`：仅 USER CODE 段——Includes 加 `#include "lcd.h"`；BEGIN 2 加 `LCD_Init();` 与 `LCD_ShowString(128,112,"我是G473",GREEN,BLACK);`
- `MDK-ARM/car_dipan.uvprojx`：新增 `Hardware` 分组；IncludePath 加 `../Hardware`

**汉字取模**：SimSun 12pt 渲染 16×16，逐行 2 字节、高位在左（已目视校验字符画）。

**已知限制 / 注意**
- CubeMX 重新 Generate 会重写 `.uvprojx`：本次 `Hardware` 分组和 `../Hardware` 路径没丢，但 `IROM1/IRAM1` 被清成 0（见上方“已修”）。**每次 Generate 后都去 Options for Target → Target 页扫一眼存储区。**
- EIDE 的内存区在 `.eide/eide.yml`，Generate 不会动它，但从 Keil 工程导入时会把空值带进来。**改完 eide.yml 要 `Developer: Reload Window` 让插件重读，否则它仍用内存里的旧值。**
- 字库目前只有“我、是”两个汉字，追加汉字需同时改 `hz_code[]` 与 `hz_16x16[]`。

**回滚**
1. 删除 `Hardware/` 整个目录
2. 删掉 `main.c` 中 USER CODE 两处新增
3. `.uvprojx` 删 `Hardware` 分组 + IncludePath 去掉 `../Hardware`

---

## 2026-10-18　上传 GitHub（QU1453/27-car-chassis）

- 仓库：https://github.com/QU1453/27-car-chassis ，**公开**。简介写中文「27电赛小车底盘 / STM32G473VET6 + 2×M2006/C610 + ST7789」。
  **GitHub 仓库名只允许 ASCII**（字母/数字/`-`/`_`/`.`），原名「27电赛小车地盘」建不出来，只能放简介。
- 新增 `.gitignore`：排除 `MDK-ARM/build`、`MDK-ARM/car_dipan`（uVision 输出）、`MDK-ARM/.cmsis`（EIDE 包缓存）、`MDK-ARM/RTE`、`*.o/.d/.crf/.axf/.hex/.map` 等；入库 117 个源文件。
- 本机执行 `git init -b main` → 首个提交 `16a453f` → 推送到 `origin/main` 并设 upstream。
- 环境备注：本机**没有 `gh` CLI**；TRAE 的 GitHub 集成令牌只读（建仓报 403），建仓靠网页手动完成，推送用该令牌走 `http.extraHeader` 认证，令牌不落盘。

