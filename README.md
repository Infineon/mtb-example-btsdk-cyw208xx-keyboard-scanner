# CYW208xx - HAL Keyboard Scanner

This code example demonstrates the dedicated keyscan hardware, which processes key press events from proprietary GPIOs (LHL GPIOs) without interrupting the CPU core. This code example also demonstrates low power entry when the system is idle (no activity or keypress) and wake up when there is key press. This code example is developed using ModusToolbox™ integrated development environment (IDE).

**Features Demonstrated:**
1. Functionality of keyscan with maximum possible buttons (that is, 63 in 7 rows * 9 columns) on CYW9208xxEVB-02.
2. Configuring Extended Power Down State (ePDS) low power mode and post wake-up.

## Requirements
- **Tool**: [ModusToolbox IDE](https://www.cypress.com/products/modustoolbox-software-environment) v2.1
- **Programming Language**: C
- **Associated Parts**: [CYW20819](https://www.cypress.com/datasheet/CYW20819), [CYW20820](https://www.cypress.com/datasheet/CYW20820)
## Supported Kits
- [CYW920819EVB-02 Evaluation Kit](http://www.cypress.com/CYW920819EVB-02)
- [CYW920820EVB-02 Evaluation kit](http://www.cypress.com/CYW920820EVB-02)

## Hardware Setup
This example uses the board's default configuration. See the kit user guide to ensure that the board is configured correctly. However, some of the default super-muxable functions are remapped to the Keyscan hardware block.


## Software Setup
Install a terminal emulator if you don't have one. Instructions in this document use [Tera Term](https://ttssh2.osdn.jp/index.html.en). All other required software comes bundled with the ModusToolbox.

## Using the Code Example

### In ModusToolbox IDE:
1. Install ModusToolbox v2.1
2. Click the **New Application** link in the Quick Panel (or, use **File** > **New** > **ModusToolbox Application**).
3. Select your board and click **Next**.
4. Select **wiced\_btsdk**. This project contains the BT-SDK. It is used by all BTSDK applications. You need to create this project once in the working directory (that is, Eclipse workspace). Ignore, if you have already created this project.

   **Note**: Do not change the name of this project. All BTSDK apps use this project name in application makefiles.

5. After the **wiced\_btsdk** project is created, click  **New Application** again, select the board, and click **Next**.
6. Select the **HAL Keyboard Scanner** application and click **Next**. Click **Create** to create the application.
7. Select the application in the IDE. In the Quick Panel, click **Build** to build the application.
8. To program the board (download the application), click **Program** in the Quick Panel.

### In Command-Line Interface (CLI):

1. Install ModusToolbox 2.x.
2. On Windows, navigate to the modus-shell directory *({ModusToolbox install directory}/tools_<version>/modus-shell)* and run *Cygwin.bat*. The default installallation folder is the user's home directory. Run all following steps from within the Cygwin window.
3. Go to the directory that you want to use for your workspace. Use ```mkdir \<directory>``` to make new directory and ```cd \<directory>``` to change to that directory.
4. Git clone `wiced_btsdk` repository first. As mentioned earlier, this project contains the BT-SDK used by all Bluetooth applications. You  need to create this project once in the working directory. For example:
   ```
   > git clone https://github.com/cypresssemiconductorco/wiced_btsdk
   ```
5. Git clone the application repository **mtb-example-cyw208xx-keyboard-scanner**. The application repository directory should be at the same folder level as **wiced_btsdk**. For example:
   ```
   > git clone https://github.com/cypresssemiconductorco/mtb-example-cyw208xx-keyboard-scanner
   ```
6. The *wiced_btsdk* repository contains references to other repositories. To download all required collateral, `cd` to the SDK root folder and use `make getlibs`. For example:
   ```
   > cd wiced_btsdk
   > make getlibs
   ```
7. To build the application, call `make build` from the application's top directory. For example, if you start from the *wiced_btsdk* directory, you would use:
   ```
   > cd ../mtb-example-cyw208xx-keyboard-scanner
   > make build
   ```
8. To program (download to) the board, use the command:
   ```
   > make qprogram
   ```
9. To build and program (download to) the board in a single step:
   ```
   > make program
   ```

   **Note**: `make program` = `make build` + `make qprogram`

## Operation

1. Connect the board to your PC using the provided USB cable through the USB connector.

2. Open a terminal program and select the COM port (generally, it is the higher numbered port). Set the serial port parameters to 8N1 and 115200 baud.

3. Program the board.

4. Follow the instructions that appear on the terminal.

##### Figure 1. PUART Messages in Tera Term
![UART Messages](images/uart_msgs.png)


## Design and Implementation

The objective of the application is to demonstrate how you can make use of the keyboard scanner block available in the CYW20xxx Bluetooth Devices. This application also exhibits the procedure to configure ePDS low power mode when the Keyscan block is inactive. This application uses custom Board Support Package (BSP/design.modus) as it overrides the default LHL GPIO pin configurations with the help of super-mux functionality. See the datasheet for more details on Super-mux functionality. The application also uses a 100 millisecond timer to track the key pressed time.

The following LHL GPIO pins are configured as keyscan rows and columns:

##### Table 1. Mapped Rows to GPIOs
  | **ROW#** |**Mapped Px#** |
  |----------|---------------|
  |  0       |    P0|
  |  1       |    P1|
  |  2       |    P2|
  |  3       |    P3|
  |  4       |    P4|
  |  5       |    P5|
  |  6       |    P6|

##### Table 2. Mapped Columns to GPIOs**
  | **COL#** |**Mapped Px#** |
  |----------|---------------|
  |  0       |    P8|
  |  1       |    P9|
  |  2       |    P10|
  |  3       |    P11|
  |  4       |    P12|
  |  5       |    P13|
  |  6       |    P14|
  |  7       |    P15|
  |  8       |    P17|


* **Device Configurator** The Device Configurator is used to enable/configure the peripherals and the pins used in the application. See the
[Device Configurator Guide](https://www.cypress.com/ModusToolboxDeviceConfig).

* **Bluetooth Configurator:** The Bluetooth Configurator is used for generating/modifying the BLE GATT database. See the
[Bluetooth Configurator Guide](https://www.cypress.com/ModusToolboxBLEConfig).

## Related Resources

| **Application Notes**                                            |                                                              |
| :----------------------------------------------------------- | :----------------------------------------------------------- |
|[AN225684 – Getting Started with CYW208XX](http://www.cypress.com/an225684) | Describes CYW20819, CYW20820 Bluetooth SoC, software/hardware development ecosystem, and how to build your first BLE application using the device in ModusToolbox IDE.
| **Code Examples**  |
|[Using ModusToolbox](https://github.com/cypresssemiconductorco/Code-Examples-for-ModusToolbox-Software)|
|**Device Documentation**|
|[CYW20819 Device Datasheet](https://www.cypress.com/datasheet/CYW20819) |[CYW20820 Device Datasheet](https://www.cypress.com/datasheet/CYW20820)|
|**Development Kits** |
|[CYW920819EVB-02 Evaluation Kit](http://www.cypress.com/CYW920819EVB-02) |[CYW920820EVB-02 Evaluation Kit](http://www.cypress.com/CYW920820EVB-02)|
|**Tool Documentation** |
|[Eclipse IDE for ModusToolbox](http://www.cypress.com/modustoolbox)| The multi-platform, Eclipse-based Integrated Development Environment (IDE) that supports application configuration and development for PSoC 6 MCU and IoT designers.|
--------------------------------------------------------------------------------------------

## Document History

Document Title: CE229928 - CYW208xx - HAL Keyboard Scanner

| Version | Description of Change |
| ------- | --------------------- |
| 1.0.0   | New code example      |

------

![Banner](images/Banner.png)

-------------------------------------------------------------------------------

© Cypress Semiconductor Corporation, 2020. This document is the property of Cypress Semiconductor Corporation and its subsidiaries (“Cypress”).  This document, including any software or firmware included or referenced in this document (“Software”), is owned by Cypress under the intellectual property laws and treaties of the United States and other countries worldwide.  Cypress reserves all rights under such laws and treaties and does not, except as specifically stated in this paragraph, grant any license under its patents, copyrights, trademarks, or other intellectual property rights.  If the Software is not accompanied by a license agreement and you do not otherwise have a written agreement with Cypress governing the use of the Software, then Cypress hereby grants you a personal, non-exclusive, nontransferable license (without the right to sublicense) (1) under its copyright rights in the Software (a) for Software provided in source code form, to modify and reproduce the Software solely for use with Cypress hardware products, only internally within your organization, and (b) to distribute the Software in binary code form externally to end users (either directly or indirectly through resellers and distributors), solely for use on Cypress hardware product units, and (2) under those claims of Cypress’s patents that are infringed by the Software (as provided by Cypress, unmodified) to make, use, distribute, and import the Software solely for use with Cypress hardware products.  Any other use, reproduction, modification, translation, or compilation of the Software is prohibited.
TO THE EXTENT PERMITTED BY APPLICABLE LAW, CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH REGARD TO THIS DOCUMENT OR ANY SOFTWARE OR ACCOMPANYING HARDWARE, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  No computing device can be absolutely secure.  Therefore, despite security measures implemented in Cypress hardware or software products, Cypress shall have no liability arising out of any security breach, such as unauthorized access to or use of a Cypress product.  CYPRESS DOES NOT REPRESENT, WARRANT, OR GUARANTEE THAT CYPRESS PRODUCTS, OR SYSTEMS CREATED USING CYPRESS PRODUCTS, WILL BE FREE FROM CORRUPTION, ATTACK, VIRUSES, INTERFERENCE, HACKING, DATA LOSS OR THEFT, OR OTHER SECURITY INTRUSION (collectively, “Security Breach”).  Cypress disclaims any liability relating to any Security Breach, and you shall and hereby do release Cypress from any claim, damage, or other liability arising from any Security Breach.  In addition, the products described in these materials may contain design defects or errors known as errata which may cause the product to deviate from published specifications.  To the extent permitted by applicable law, Cypress reserves the right to make changes to this document without further notice. Cypress does not assume any liability arising out of the application or use of any product or circuit described in this document.  Any information provided in this document, including any sample design information or programming code, is provided only for reference purposes.  It is the responsibility of the user of this document to properly design, program, and test the functionality and safety of any application made of this information and any resulting product.  “High-Risk Device” means any device or system whose failure could cause personal injury, death, or property damage.  Examples of High-Risk Devices are weapons, nuclear installations, surgical implants, and other medical devices.  “Critical Component” means any component of a High-Risk Device whose failure to perform can be reasonably expected to cause, directly or indirectly, the failure of the High-Risk Device, or to affect its safety or effectiveness.  Cypress is not liable, in whole or in part, and you shall and hereby do release Cypress from any claim, damage, or other liability arising from any use of a Cypress product as a Critical Component in a High-Risk Device.  You shall indemnify and hold Cypress, its directors, officers, employees, agents, affiliates, distributors, and assigns harmless from and against all claims, costs, damages, and expenses, arising out of any claim, including claims for product liability, personal injury or death, or property damage arising from any use of a Cypress product as a Critical Component in a High-Risk Device.  Cypress products are not intended or authorized for use as a Critical Component in any High-Risk Device except to the limited extent that (i) Cypress’s published data sheet for the product explicitly states Cypress has qualified the product for use in a specific High-Risk Device, or (ii) Cypress has given you advance written authorization to use the product as a Critical Component in the specific High-Risk Device and you have signed a separate indemnification agreement.
Cypress, the Cypress logo, Spansion, the Spansion logo, and combinations thereof, WICED, PSoC, CapSense, EZ-USB, F-RAM, and Traveo are trademarks or registered trademarks of Cypress in the United States and other countries.  For a more complete list of Cypress trademarks, visit cypress.com.  Other names and brands may be claimed as property of their respective owners.
