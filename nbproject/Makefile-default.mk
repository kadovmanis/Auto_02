#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=mkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/Auto_02.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/Auto_02.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=-mafrlcsj
else
COMPARISON_BUILD=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=src/Adc.c src/Boot.c src/Gps.c src/Gsm.c src/Main.c src/Memory.c src/Ports.c src/Spi_i2c.c src/Tcp.c src/Timers.c src/Uart1.c src/Uart2.c src/Uart3.c src/Uart4.c src/Wifi.c Usb/app_device_custom_hid.c Usb/app_led_usb_status.c Usb/system.c Usb/usb_descriptors.c Usb/src/usb_device.c Usb/src/usb_device_hid.c src/Font_7x5.c src/Font_SansSerif_16.c src/Font_segoe_24.c src/Lcd.c src/Lcd_Fonts.c src/Lcd_rgb.c src/W5100.c src/Input.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/src/Adc.o ${OBJECTDIR}/src/Boot.o ${OBJECTDIR}/src/Gps.o ${OBJECTDIR}/src/Gsm.o ${OBJECTDIR}/src/Main.o ${OBJECTDIR}/src/Memory.o ${OBJECTDIR}/src/Ports.o ${OBJECTDIR}/src/Spi_i2c.o ${OBJECTDIR}/src/Tcp.o ${OBJECTDIR}/src/Timers.o ${OBJECTDIR}/src/Uart1.o ${OBJECTDIR}/src/Uart2.o ${OBJECTDIR}/src/Uart3.o ${OBJECTDIR}/src/Uart4.o ${OBJECTDIR}/src/Wifi.o ${OBJECTDIR}/Usb/app_device_custom_hid.o ${OBJECTDIR}/Usb/app_led_usb_status.o ${OBJECTDIR}/Usb/system.o ${OBJECTDIR}/Usb/usb_descriptors.o ${OBJECTDIR}/Usb/src/usb_device.o ${OBJECTDIR}/Usb/src/usb_device_hid.o ${OBJECTDIR}/src/Font_7x5.o ${OBJECTDIR}/src/Font_SansSerif_16.o ${OBJECTDIR}/src/Font_segoe_24.o ${OBJECTDIR}/src/Lcd.o ${OBJECTDIR}/src/Lcd_Fonts.o ${OBJECTDIR}/src/Lcd_rgb.o ${OBJECTDIR}/src/W5100.o ${OBJECTDIR}/src/Input.o
POSSIBLE_DEPFILES=${OBJECTDIR}/src/Adc.o.d ${OBJECTDIR}/src/Boot.o.d ${OBJECTDIR}/src/Gps.o.d ${OBJECTDIR}/src/Gsm.o.d ${OBJECTDIR}/src/Main.o.d ${OBJECTDIR}/src/Memory.o.d ${OBJECTDIR}/src/Ports.o.d ${OBJECTDIR}/src/Spi_i2c.o.d ${OBJECTDIR}/src/Tcp.o.d ${OBJECTDIR}/src/Timers.o.d ${OBJECTDIR}/src/Uart1.o.d ${OBJECTDIR}/src/Uart2.o.d ${OBJECTDIR}/src/Uart3.o.d ${OBJECTDIR}/src/Uart4.o.d ${OBJECTDIR}/src/Wifi.o.d ${OBJECTDIR}/Usb/app_device_custom_hid.o.d ${OBJECTDIR}/Usb/app_led_usb_status.o.d ${OBJECTDIR}/Usb/system.o.d ${OBJECTDIR}/Usb/usb_descriptors.o.d ${OBJECTDIR}/Usb/src/usb_device.o.d ${OBJECTDIR}/Usb/src/usb_device_hid.o.d ${OBJECTDIR}/src/Font_7x5.o.d ${OBJECTDIR}/src/Font_SansSerif_16.o.d ${OBJECTDIR}/src/Font_segoe_24.o.d ${OBJECTDIR}/src/Lcd.o.d ${OBJECTDIR}/src/Lcd_Fonts.o.d ${OBJECTDIR}/src/Lcd_rgb.o.d ${OBJECTDIR}/src/W5100.o.d ${OBJECTDIR}/src/Input.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/src/Adc.o ${OBJECTDIR}/src/Boot.o ${OBJECTDIR}/src/Gps.o ${OBJECTDIR}/src/Gsm.o ${OBJECTDIR}/src/Main.o ${OBJECTDIR}/src/Memory.o ${OBJECTDIR}/src/Ports.o ${OBJECTDIR}/src/Spi_i2c.o ${OBJECTDIR}/src/Tcp.o ${OBJECTDIR}/src/Timers.o ${OBJECTDIR}/src/Uart1.o ${OBJECTDIR}/src/Uart2.o ${OBJECTDIR}/src/Uart3.o ${OBJECTDIR}/src/Uart4.o ${OBJECTDIR}/src/Wifi.o ${OBJECTDIR}/Usb/app_device_custom_hid.o ${OBJECTDIR}/Usb/app_led_usb_status.o ${OBJECTDIR}/Usb/system.o ${OBJECTDIR}/Usb/usb_descriptors.o ${OBJECTDIR}/Usb/src/usb_device.o ${OBJECTDIR}/Usb/src/usb_device_hid.o ${OBJECTDIR}/src/Font_7x5.o ${OBJECTDIR}/src/Font_SansSerif_16.o ${OBJECTDIR}/src/Font_segoe_24.o ${OBJECTDIR}/src/Lcd.o ${OBJECTDIR}/src/Lcd_Fonts.o ${OBJECTDIR}/src/Lcd_rgb.o ${OBJECTDIR}/src/W5100.o ${OBJECTDIR}/src/Input.o

# Source Files
SOURCEFILES=src/Adc.c src/Boot.c src/Gps.c src/Gsm.c src/Main.c src/Memory.c src/Ports.c src/Spi_i2c.c src/Tcp.c src/Timers.c src/Uart1.c src/Uart2.c src/Uart3.c src/Uart4.c src/Wifi.c Usb/app_device_custom_hid.c Usb/app_led_usb_status.c Usb/system.c Usb/usb_descriptors.c Usb/src/usb_device.c Usb/src/usb_device_hid.c src/Font_7x5.c src/Font_SansSerif_16.c src/Font_segoe_24.c src/Lcd.c src/Lcd_Fonts.c src/Lcd_rgb.c src/W5100.c src/Input.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

# The following macros may be used in the pre and post step lines
Device=PIC24FJ128GB206
ProjectDir=/home/pele/src/Auto_02
ConfName=default
ImagePath=dist/default/${IMAGE_TYPE}/Auto_02.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
ImageDir=dist/default/${IMAGE_TYPE}
ImageName=Auto_02.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IsDebug="true"
else
IsDebug="false"
endif

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/Auto_02.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
	@echo "--------------------------------------"
	@echo "User defined post-build step: [xc32-objcopy -I ihex -O binary dist/default/production/Auto_02.production.hex Auto_02.bin]"
	@xc32-objcopy -I ihex -O binary dist/default/production/Auto_02.production.hex Auto_02.bin
	@echo "--------------------------------------"

MP_PROCESSOR_OPTION=24FJ128GB206
MP_LINKER_FILE_OPTION=,--script="lnk/p24FJ128GB206_boot.gld"
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/src/Adc.o: src/Adc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Adc.o.d 
	@${RM} ${OBJECTDIR}/src/Adc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Adc.c  -o ${OBJECTDIR}/src/Adc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Adc.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Adc.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Boot.o: src/Boot.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Boot.o.d 
	@${RM} ${OBJECTDIR}/src/Boot.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Boot.c  -o ${OBJECTDIR}/src/Boot.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Boot.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Boot.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Gps.o: src/Gps.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Gps.o.d 
	@${RM} ${OBJECTDIR}/src/Gps.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Gps.c  -o ${OBJECTDIR}/src/Gps.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Gps.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Gps.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Gsm.o: src/Gsm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Gsm.o.d 
	@${RM} ${OBJECTDIR}/src/Gsm.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Gsm.c  -o ${OBJECTDIR}/src/Gsm.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Gsm.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Gsm.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Main.o: src/Main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Main.o.d 
	@${RM} ${OBJECTDIR}/src/Main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Main.c  -o ${OBJECTDIR}/src/Main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Main.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Memory.o: src/Memory.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Memory.o.d 
	@${RM} ${OBJECTDIR}/src/Memory.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Memory.c  -o ${OBJECTDIR}/src/Memory.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Memory.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Memory.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Ports.o: src/Ports.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Ports.o.d 
	@${RM} ${OBJECTDIR}/src/Ports.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Ports.c  -o ${OBJECTDIR}/src/Ports.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Ports.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Ports.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Spi_i2c.o: src/Spi_i2c.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Spi_i2c.o.d 
	@${RM} ${OBJECTDIR}/src/Spi_i2c.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Spi_i2c.c  -o ${OBJECTDIR}/src/Spi_i2c.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Spi_i2c.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Spi_i2c.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Tcp.o: src/Tcp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Tcp.o.d 
	@${RM} ${OBJECTDIR}/src/Tcp.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Tcp.c  -o ${OBJECTDIR}/src/Tcp.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Tcp.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Tcp.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Timers.o: src/Timers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Timers.o.d 
	@${RM} ${OBJECTDIR}/src/Timers.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Timers.c  -o ${OBJECTDIR}/src/Timers.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Timers.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Timers.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Uart1.o: src/Uart1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Uart1.o.d 
	@${RM} ${OBJECTDIR}/src/Uart1.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Uart1.c  -o ${OBJECTDIR}/src/Uart1.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Uart1.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Uart1.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Uart2.o: src/Uart2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Uart2.o.d 
	@${RM} ${OBJECTDIR}/src/Uart2.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Uart2.c  -o ${OBJECTDIR}/src/Uart2.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Uart2.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Uart2.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Uart3.o: src/Uart3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Uart3.o.d 
	@${RM} ${OBJECTDIR}/src/Uart3.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Uart3.c  -o ${OBJECTDIR}/src/Uart3.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Uart3.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Uart3.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Uart4.o: src/Uart4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Uart4.o.d 
	@${RM} ${OBJECTDIR}/src/Uart4.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Uart4.c  -o ${OBJECTDIR}/src/Uart4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Uart4.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Uart4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Wifi.o: src/Wifi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Wifi.o.d 
	@${RM} ${OBJECTDIR}/src/Wifi.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Wifi.c  -o ${OBJECTDIR}/src/Wifi.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Wifi.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Wifi.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Usb/app_device_custom_hid.o: Usb/app_device_custom_hid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Usb" 
	@${RM} ${OBJECTDIR}/Usb/app_device_custom_hid.o.d 
	@${RM} ${OBJECTDIR}/Usb/app_device_custom_hid.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Usb/app_device_custom_hid.c  -o ${OBJECTDIR}/Usb/app_device_custom_hid.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Usb/app_device_custom_hid.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Usb/app_device_custom_hid.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Usb/app_led_usb_status.o: Usb/app_led_usb_status.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Usb" 
	@${RM} ${OBJECTDIR}/Usb/app_led_usb_status.o.d 
	@${RM} ${OBJECTDIR}/Usb/app_led_usb_status.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Usb/app_led_usb_status.c  -o ${OBJECTDIR}/Usb/app_led_usb_status.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Usb/app_led_usb_status.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Usb/app_led_usb_status.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Usb/system.o: Usb/system.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Usb" 
	@${RM} ${OBJECTDIR}/Usb/system.o.d 
	@${RM} ${OBJECTDIR}/Usb/system.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Usb/system.c  -o ${OBJECTDIR}/Usb/system.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Usb/system.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Usb/system.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Usb/usb_descriptors.o: Usb/usb_descriptors.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Usb" 
	@${RM} ${OBJECTDIR}/Usb/usb_descriptors.o.d 
	@${RM} ${OBJECTDIR}/Usb/usb_descriptors.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Usb/usb_descriptors.c  -o ${OBJECTDIR}/Usb/usb_descriptors.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Usb/usb_descriptors.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Usb/usb_descriptors.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Usb/src/usb_device.o: Usb/src/usb_device.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Usb/src" 
	@${RM} ${OBJECTDIR}/Usb/src/usb_device.o.d 
	@${RM} ${OBJECTDIR}/Usb/src/usb_device.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Usb/src/usb_device.c  -o ${OBJECTDIR}/Usb/src/usb_device.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Usb/src/usb_device.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Usb/src/usb_device.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Usb/src/usb_device_hid.o: Usb/src/usb_device_hid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Usb/src" 
	@${RM} ${OBJECTDIR}/Usb/src/usb_device_hid.o.d 
	@${RM} ${OBJECTDIR}/Usb/src/usb_device_hid.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Usb/src/usb_device_hid.c  -o ${OBJECTDIR}/Usb/src/usb_device_hid.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Usb/src/usb_device_hid.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Usb/src/usb_device_hid.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Font_7x5.o: src/Font_7x5.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Font_7x5.o.d 
	@${RM} ${OBJECTDIR}/src/Font_7x5.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Font_7x5.c  -o ${OBJECTDIR}/src/Font_7x5.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Font_7x5.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Font_7x5.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Font_SansSerif_16.o: src/Font_SansSerif_16.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Font_SansSerif_16.o.d 
	@${RM} ${OBJECTDIR}/src/Font_SansSerif_16.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Font_SansSerif_16.c  -o ${OBJECTDIR}/src/Font_SansSerif_16.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Font_SansSerif_16.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Font_SansSerif_16.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Font_segoe_24.o: src/Font_segoe_24.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Font_segoe_24.o.d 
	@${RM} ${OBJECTDIR}/src/Font_segoe_24.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Font_segoe_24.c  -o ${OBJECTDIR}/src/Font_segoe_24.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Font_segoe_24.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Font_segoe_24.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Lcd.o: src/Lcd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Lcd.o.d 
	@${RM} ${OBJECTDIR}/src/Lcd.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Lcd.c  -o ${OBJECTDIR}/src/Lcd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Lcd.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Lcd.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Lcd_Fonts.o: src/Lcd_Fonts.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Lcd_Fonts.o.d 
	@${RM} ${OBJECTDIR}/src/Lcd_Fonts.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Lcd_Fonts.c  -o ${OBJECTDIR}/src/Lcd_Fonts.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Lcd_Fonts.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Lcd_Fonts.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Lcd_rgb.o: src/Lcd_rgb.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Lcd_rgb.o.d 
	@${RM} ${OBJECTDIR}/src/Lcd_rgb.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Lcd_rgb.c  -o ${OBJECTDIR}/src/Lcd_rgb.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Lcd_rgb.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Lcd_rgb.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/W5100.o: src/W5100.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/W5100.o.d 
	@${RM} ${OBJECTDIR}/src/W5100.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/W5100.c  -o ${OBJECTDIR}/src/W5100.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/W5100.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/W5100.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Input.o: src/Input.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Input.o.d 
	@${RM} ${OBJECTDIR}/src/Input.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Input.c  -o ${OBJECTDIR}/src/Input.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Input.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Input.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/src/Adc.o: src/Adc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Adc.o.d 
	@${RM} ${OBJECTDIR}/src/Adc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Adc.c  -o ${OBJECTDIR}/src/Adc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Adc.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Adc.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Boot.o: src/Boot.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Boot.o.d 
	@${RM} ${OBJECTDIR}/src/Boot.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Boot.c  -o ${OBJECTDIR}/src/Boot.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Boot.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Boot.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Gps.o: src/Gps.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Gps.o.d 
	@${RM} ${OBJECTDIR}/src/Gps.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Gps.c  -o ${OBJECTDIR}/src/Gps.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Gps.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Gps.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Gsm.o: src/Gsm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Gsm.o.d 
	@${RM} ${OBJECTDIR}/src/Gsm.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Gsm.c  -o ${OBJECTDIR}/src/Gsm.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Gsm.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Gsm.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Main.o: src/Main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Main.o.d 
	@${RM} ${OBJECTDIR}/src/Main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Main.c  -o ${OBJECTDIR}/src/Main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Main.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Memory.o: src/Memory.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Memory.o.d 
	@${RM} ${OBJECTDIR}/src/Memory.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Memory.c  -o ${OBJECTDIR}/src/Memory.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Memory.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Memory.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Ports.o: src/Ports.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Ports.o.d 
	@${RM} ${OBJECTDIR}/src/Ports.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Ports.c  -o ${OBJECTDIR}/src/Ports.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Ports.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Ports.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Spi_i2c.o: src/Spi_i2c.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Spi_i2c.o.d 
	@${RM} ${OBJECTDIR}/src/Spi_i2c.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Spi_i2c.c  -o ${OBJECTDIR}/src/Spi_i2c.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Spi_i2c.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Spi_i2c.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Tcp.o: src/Tcp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Tcp.o.d 
	@${RM} ${OBJECTDIR}/src/Tcp.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Tcp.c  -o ${OBJECTDIR}/src/Tcp.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Tcp.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Tcp.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Timers.o: src/Timers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Timers.o.d 
	@${RM} ${OBJECTDIR}/src/Timers.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Timers.c  -o ${OBJECTDIR}/src/Timers.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Timers.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Timers.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Uart1.o: src/Uart1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Uart1.o.d 
	@${RM} ${OBJECTDIR}/src/Uart1.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Uart1.c  -o ${OBJECTDIR}/src/Uart1.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Uart1.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Uart1.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Uart2.o: src/Uart2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Uart2.o.d 
	@${RM} ${OBJECTDIR}/src/Uart2.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Uart2.c  -o ${OBJECTDIR}/src/Uart2.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Uart2.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Uart2.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Uart3.o: src/Uart3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Uart3.o.d 
	@${RM} ${OBJECTDIR}/src/Uart3.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Uart3.c  -o ${OBJECTDIR}/src/Uart3.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Uart3.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Uart3.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Uart4.o: src/Uart4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Uart4.o.d 
	@${RM} ${OBJECTDIR}/src/Uart4.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Uart4.c  -o ${OBJECTDIR}/src/Uart4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Uart4.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Uart4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Wifi.o: src/Wifi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Wifi.o.d 
	@${RM} ${OBJECTDIR}/src/Wifi.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Wifi.c  -o ${OBJECTDIR}/src/Wifi.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Wifi.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Wifi.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Usb/app_device_custom_hid.o: Usb/app_device_custom_hid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Usb" 
	@${RM} ${OBJECTDIR}/Usb/app_device_custom_hid.o.d 
	@${RM} ${OBJECTDIR}/Usb/app_device_custom_hid.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Usb/app_device_custom_hid.c  -o ${OBJECTDIR}/Usb/app_device_custom_hid.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Usb/app_device_custom_hid.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Usb/app_device_custom_hid.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Usb/app_led_usb_status.o: Usb/app_led_usb_status.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Usb" 
	@${RM} ${OBJECTDIR}/Usb/app_led_usb_status.o.d 
	@${RM} ${OBJECTDIR}/Usb/app_led_usb_status.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Usb/app_led_usb_status.c  -o ${OBJECTDIR}/Usb/app_led_usb_status.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Usb/app_led_usb_status.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Usb/app_led_usb_status.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Usb/system.o: Usb/system.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Usb" 
	@${RM} ${OBJECTDIR}/Usb/system.o.d 
	@${RM} ${OBJECTDIR}/Usb/system.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Usb/system.c  -o ${OBJECTDIR}/Usb/system.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Usb/system.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Usb/system.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Usb/usb_descriptors.o: Usb/usb_descriptors.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Usb" 
	@${RM} ${OBJECTDIR}/Usb/usb_descriptors.o.d 
	@${RM} ${OBJECTDIR}/Usb/usb_descriptors.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Usb/usb_descriptors.c  -o ${OBJECTDIR}/Usb/usb_descriptors.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Usb/usb_descriptors.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Usb/usb_descriptors.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Usb/src/usb_device.o: Usb/src/usb_device.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Usb/src" 
	@${RM} ${OBJECTDIR}/Usb/src/usb_device.o.d 
	@${RM} ${OBJECTDIR}/Usb/src/usb_device.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Usb/src/usb_device.c  -o ${OBJECTDIR}/Usb/src/usb_device.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Usb/src/usb_device.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Usb/src/usb_device.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Usb/src/usb_device_hid.o: Usb/src/usb_device_hid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Usb/src" 
	@${RM} ${OBJECTDIR}/Usb/src/usb_device_hid.o.d 
	@${RM} ${OBJECTDIR}/Usb/src/usb_device_hid.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Usb/src/usb_device_hid.c  -o ${OBJECTDIR}/Usb/src/usb_device_hid.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Usb/src/usb_device_hid.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Usb/src/usb_device_hid.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Font_7x5.o: src/Font_7x5.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Font_7x5.o.d 
	@${RM} ${OBJECTDIR}/src/Font_7x5.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Font_7x5.c  -o ${OBJECTDIR}/src/Font_7x5.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Font_7x5.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Font_7x5.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Font_SansSerif_16.o: src/Font_SansSerif_16.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Font_SansSerif_16.o.d 
	@${RM} ${OBJECTDIR}/src/Font_SansSerif_16.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Font_SansSerif_16.c  -o ${OBJECTDIR}/src/Font_SansSerif_16.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Font_SansSerif_16.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Font_SansSerif_16.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Font_segoe_24.o: src/Font_segoe_24.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Font_segoe_24.o.d 
	@${RM} ${OBJECTDIR}/src/Font_segoe_24.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Font_segoe_24.c  -o ${OBJECTDIR}/src/Font_segoe_24.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Font_segoe_24.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Font_segoe_24.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Lcd.o: src/Lcd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Lcd.o.d 
	@${RM} ${OBJECTDIR}/src/Lcd.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Lcd.c  -o ${OBJECTDIR}/src/Lcd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Lcd.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Lcd.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Lcd_Fonts.o: src/Lcd_Fonts.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Lcd_Fonts.o.d 
	@${RM} ${OBJECTDIR}/src/Lcd_Fonts.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Lcd_Fonts.c  -o ${OBJECTDIR}/src/Lcd_Fonts.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Lcd_Fonts.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Lcd_Fonts.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Lcd_rgb.o: src/Lcd_rgb.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Lcd_rgb.o.d 
	@${RM} ${OBJECTDIR}/src/Lcd_rgb.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Lcd_rgb.c  -o ${OBJECTDIR}/src/Lcd_rgb.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Lcd_rgb.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Lcd_rgb.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/W5100.o: src/W5100.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/W5100.o.d 
	@${RM} ${OBJECTDIR}/src/W5100.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/W5100.c  -o ${OBJECTDIR}/src/W5100.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/W5100.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/W5100.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/Input.o: src/Input.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/Input.o.d 
	@${RM} ${OBJECTDIR}/src/Input.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/Input.c  -o ${OBJECTDIR}/src/Input.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/Input.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/Input.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/Auto_02.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    lnk/p24FJ128GB206_boot.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/Auto_02.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" --fill=0xFFFFFF  -mreserve=data@0x800:0x81B -mreserve=data@0x81C:0x81D -mreserve=data@0x81E:0x81F -mreserve=data@0x820:0x821 -mreserve=data@0x822:0x823 -mreserve=data@0x824:0x827 -mreserve=data@0x82A:0x84F   -Wl,--local-stack,,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--cref,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST) 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/Auto_02.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   lnk/p24FJ128GB206_boot.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/Auto_02.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"h" -I"Usb" -I"Usb/inc" -I"Usb/src" --fill=0xFFFFFF -Wl,--local-stack,,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--cref,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST) 
	${MP_CC_DIR}/xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/Auto_02.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf  
	
	@echo Normalizing hex file
	@"/opt/microchip/mplabx/v3.55/mplab_ide/platform/../mplab_ide/modules/../../bin/hexmate" --edf="/opt/microchip/mplabx/v3.55/mplab_ide/platform/../mplab_ide/modules/../../dat/en_msgs.txt" dist/${CND_CONF}/${IMAGE_TYPE}/Auto_02.${IMAGE_TYPE}.hex -odist/${CND_CONF}/${IMAGE_TYPE}/Auto_02.${IMAGE_TYPE}.hex

endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell "${PATH_TO_IDE_BIN}"mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
