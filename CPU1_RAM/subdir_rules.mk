################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
build-1622975546: ../c2000.syscfg
	@echo 'SysConfig - building file: "$<"'
	"C:/ti/ccs2101/ccs/utils/sysconfig_1.28.1/sysconfig_cli.bat" -s "C:/ti/c2000/C2000Ware_26_02_00_00/.metadata/sdk.json" -b "/boards/LAUNCHXL_F28P65X" --script "C:/Users/Steve/workspace_ccstheia/empty_driverlib_project/c2000.syscfg" --context "CPU1" -o "syscfg" --compiler ccs
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/board.c: build-1622975546 ../c2000.syscfg
syscfg/board.h: build-1622975546
syscfg/board.cmd.genlibs: build-1622975546
syscfg/board.opt: build-1622975546
syscfg/board.json: build-1622975546
syscfg/pinmux.csv: build-1622975546
syscfg/c2000ware_libraries.cmd.genlibs: build-1622975546
syscfg/c2000ware_libraries.opt: build-1622975546
syscfg/c2000ware_libraries.c: build-1622975546
syscfg/c2000ware_libraries.h: build-1622975546
syscfg/clocktree.h: build-1622975546
syscfg: build-1622975546

syscfg/%.obj: ./syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'C2000 Compiler - building file: "$<"'
	"C:/ti/ccs2101/ccs/tools/compiler/ti-cgt-c2000_25.11.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla2 --float_support=fpu64 --tmu_support=tmu1 --vcu_support=vcrc -Ooff --include_path="C:/Users/Steve/workspace_ccstheia/empty_driverlib_project" --include_path="C:/ti/c2000/C2000Ware_26_02_00_00" --include_path="C:/Users/Steve/workspace_ccstheia/empty_driverlib_project/device" --include_path="C:/ti/c2000/C2000Ware_26_02_00_00/driverlib/f28p65x/driverlib/" --include_path="C:/ti/ccs2101/ccs/tools/compiler/ti-cgt-c2000_25.11.1.LTS/include" --define=RAM --define=DEBUG --define=CPU1 --diag_suppress=10063 --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="syscfg/$(basename $(<F)).d_raw" --include_path="C:/Users/Steve/workspace_ccstheia/empty_driverlib_project/CPU1_RAM/syscfg" --obj_directory="syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'C2000 Compiler - building file: "$<"'
	"C:/ti/ccs2101/ccs/tools/compiler/ti-cgt-c2000_25.11.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla2 --float_support=fpu64 --tmu_support=tmu1 --vcu_support=vcrc -Ooff --include_path="C:/Users/Steve/workspace_ccstheia/empty_driverlib_project" --include_path="C:/ti/c2000/C2000Ware_26_02_00_00" --include_path="C:/Users/Steve/workspace_ccstheia/empty_driverlib_project/device" --include_path="C:/ti/c2000/C2000Ware_26_02_00_00/driverlib/f28p65x/driverlib/" --include_path="C:/ti/ccs2101/ccs/tools/compiler/ti-cgt-c2000_25.11.1.LTS/include" --define=RAM --define=DEBUG --define=CPU1 --diag_suppress=10063 --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" --include_path="C:/Users/Steve/workspace_ccstheia/empty_driverlib_project/CPU1_RAM/syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


