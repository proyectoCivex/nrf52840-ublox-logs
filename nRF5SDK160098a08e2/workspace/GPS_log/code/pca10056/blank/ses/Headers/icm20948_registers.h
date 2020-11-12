#ifndef _ICM20948_REGISTERS_H
#define _ICM20948_REGISTERS_H


/************TYPE DEFINITIONS FOR THE RESGISTERS - BANK1*************************/

struct ICM_20948_USER_CTRL_BITS{
  uint8_t reserved_0 		: 1;
  uint8_t I2C_MST_RST		: 1;
  uint8_t SRAM_RST		: 1;
  uint8_t DMP_RST               : 1;
  uint8_t I2C_IF_DIS		: 1;
  uint8_t I2C_MST_EN		: 1;
  uint8_t FIFO_EN 		: 1;
  uint8_t DMP_EN 		: 1;
};

union ICM_20948_USER_CTRL_REG{
    uint8_t all;
    struct ICM_20948_USER_CTRL_BITS bit;
};


struct ICM_20948_LP_CONFIG_BITS{
  uint8_t reserved_0 		: 4;
  uint8_t GYRO_CYCLE		: 1;
  uint8_t ACCEL_CYCLE		: 1;
  uint8_t I2C_MST_CYCLE         : 1;
  uint8_t reserved_1		: 1;
};

union ICM_20948_LP_CONFIG_REG{
    uint8_t all;
    struct ICM_20948_LP_CONFIG_BITS bit;
};

struct ICM_20948_PWR_MGMT_1_BITS{     //bit description
  uint8_t CLKSEL		: 3;    //2:0
  uint8_t TEMP_DIS		: 1;    //3
  uint8_t reserved_0		: 1;    //4
  uint8_t LP_EN			: 1;    //5
  uint8_t SLEEP			: 1;    //6
  uint8_t DEVICE_RESET		: 1;    //7
};

union ICM_20948_PWR_MGMT_1_REG{
    uint8_t all;
    struct ICM_20948_PWR_MGMT_1_BITS bit;
};

struct ICM_20948_INT_ENABLE_BITS{     //bit description
  uint8_t I2C_MST_INT_EN	: 1;    //0
  uint8_t DMP_INT1_EN		: 1;    //1
  uint8_t PLL_RDY_EN		: 1;    //2
  uint8_t WOM_INT_EN		: 1;    //3
  uint8_t reserved		: 3;    //6:4
  uint8_t REG_WOF_EN		: 1;    //7
};

union ICM_20948_INT_ENABLE_REG{
    uint8_t all;
    struct ICM_20948_INT_ENABLE_BITS bit;
};

struct ICM_20948_GYRO_CONFIG_1_BITS{     //bit description
   uint8_t GYRO_FCHOICE     :1;         //0
   uint8_t GYRO_FS_SEL      :2;          //2:1
   uint8_t GYRO_DLPCFG      :3;          //5:3
   uint8_t reserved         :2;           //7:6
};

union ICM_20948_GYRO_CONFIG_1_REG{
    uint8_t all;
    struct ICM_20948_GYRO_CONFIG_1_BITS bit;
};


struct ICM_20948_GYRO_CONFIG_2_BITS{     //bit description
   uint8_t GYRO_AVGCF     :3;         //2:0
   uint8_t ZGYRO_CTEN     :1;         //3
   uint8_t YGYRO_CTEN     :1;         //4
   uint8_t XGYRO_CTEN     :1;         //5
   uint8_t  reserved      :2;         //7:6
};

union ICM_20948_GYRO_CONFIG_2_REG{
    uint8_t all;
    struct ICM_20948_GYRO_CONFIG_2_BITS bit;
};



struct ICM_20948_ACCEL_INTEL_CTRL_BITS{     //bit description
   uint8_t ACCEL_INTEL_MODE_INT   :1;         //0
   uint8_t ACCEL_INTEL_EN         :1;          //1
   uint8_t reserved               :6;           //7:2
};

union ICM_20948_ACCEL_INTEL_CTRL_REGS{
    uint8_t all;
    struct ICM_20948_ACCEL_INTEL_CTRL_BITS bit;
};



struct ICM_20948_ACCEL_CONFIG_BITS{     //bit description
   uint8_t ACCEL_FCHOICE     :1;         //0
   uint8_t ACCEL_FS_SEL      :2;          //2:1
   uint8_t ACCEL_DLPCFG      :3;          //5:3
   uint8_t reserved          :2;           //7:6
};

union ICM_20948_ACCEL_CONFIG_REGS{
    uint8_t all;
    struct ICM_20948_ACCEL_CONFIG_BITS bit;
};


struct ICM_20948_ACCEL_CONFIG_2_BITS{     //bit description
   uint8_t DEC3_CFG          :2;         //0:1
   uint8_t AZ_ST_EN_REG      :1;          //2
   uint8_t AY_ST_EN_REG      :1;          //3
   uint8_t AX_ST_EN_REG      :1;          //4
   uint8_t reserved          :3;           //7:5
};

union ICM_20948_ACCEL_CONFIG_2_REGS{
    uint8_t all;
    struct ICM_20948_ACCEL_CONFIG_2_BITS bit;
};


struct ICM_20948_FIFO_EN_2_BITS{     //bit description
   uint8_t TEMP_FIFO_EN        :1;         //0
   uint8_t GYRO_X_FIFO_EN      :1;        //1
   uint8_t GYRO_Y_FIFO_EN      :1;        //2
   uint8_t GYRO_Z_FIFO_EN      :1;        //3
   uint8_t ACCEL_FIFO_EN       :1;        //4
   uint8_t reserved            :3;         //7:5
};

union ICM_20948_FIFO_EN_2_BITS_REGS{
    uint8_t all;
    struct ICM_20948_FIFO_EN_2_BITS bit;
};



struct ICM_20948 {
  union	ICM_20948_USER_CTRL_REG           USER_CTRL_REG;
  union	ICM_20948_LP_CONFIG_REG           LP_CONFIG_REG;
  union	ICM_20948_PWR_MGMT_1_REG          PWR_MGMT1_REG;
  union ICM_20948_INT_ENABLE_REG          INT_ENABLE_REG;
  union ICM_20948_GYRO_CONFIG_1_REG       GYRO_CONFIG_1_REG;
  union ICM_20948_GYRO_CONFIG_2_REG       GYRO_CONFIG_2_REG;
  union ICM_20948_ACCEL_CONFIG_REGS       ACCEL_CONFIG_REG;
  union ICM_20948_ACCEL_CONFIG_2_REGS     ACCEL_CONFIG_2_REGS;
  union ICM_20948_ACCEL_INTEL_CTRL_REGS   ACCEL_INTEL_CTRL_REGS;
  union ICM_20948_FIFO_EN_2_BITS_REGS     FIFO_EN_2_BITS_REGS;
};









/**************************/



#endif