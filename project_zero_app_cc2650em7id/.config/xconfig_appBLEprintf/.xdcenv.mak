#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = C:/ti/tirtos_cc13xx_cc26xx_2_20_01_08/packages;C:/ti/tirtos_cc13xx_cc26xx_2_20_01_08/products/tidrivers_cc13xx_cc26xx_2_20_01_10/packages;C:/ti/tirtos_cc13xx_cc26xx_2_20_01_08/products/bios_6_46_01_38/packages;C:/ti/tirtos_cc13xx_cc26xx_2_20_01_08/products/uia_2_00_06_52/packages;C:/ti/tirex-content/tirtos_simplelink_2_13_00_06/packages;C:/ti/tirex-content/tirtos_simplelink_2_13_00_06/products/bios_6_42_00_08/packages;C:/ti/tirex-content/tirtos_simplelink_2_13_00_06/products/uia_2_00_02_39/packages;C:/ti/ccsv7/ccs_base;C:/Users/FireGoris/Desktop/harambe_mcu/project_zero_app_cc2650em7id/.config
override XDCROOT = C:/ti/xdctools_3_32_00_06_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = C:/ti/tirtos_cc13xx_cc26xx_2_20_01_08/packages;C:/ti/tirtos_cc13xx_cc26xx_2_20_01_08/products/tidrivers_cc13xx_cc26xx_2_20_01_10/packages;C:/ti/tirtos_cc13xx_cc26xx_2_20_01_08/products/bios_6_46_01_38/packages;C:/ti/tirtos_cc13xx_cc26xx_2_20_01_08/products/uia_2_00_06_52/packages;C:/ti/tirex-content/tirtos_simplelink_2_13_00_06/packages;C:/ti/tirex-content/tirtos_simplelink_2_13_00_06/products/bios_6_42_00_08/packages;C:/ti/tirex-content/tirtos_simplelink_2_13_00_06/products/uia_2_00_02_39/packages;C:/ti/ccsv7/ccs_base;C:/Users/FireGoris/Desktop/harambe_mcu/project_zero_app_cc2650em7id/.config;C:/ti/xdctools_3_32_00_06_core/packages;..
HOSTOS = Windows
endif
