
#include "fault_gates.h"
#include "main.h"

void initFaultGates() {
  GPIO_InitTypeDef gpioinit;

  FAULT_GATES_CLK_ENABLE();

  gpioinit.Pin   = BMS_FAULT_GATE_PIN;
  gpioinit.Mode  = GPIO_MODE_INPUT;
  gpioinit.Pull  = GPIO_PULLUP;
  gpioinit.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(FAULT_GATES_PORT, &gpioinit);

  gpioinit.Pin   = BPD_FAULT_GATE_PIN;
  gpioinit.Mode  = GPIO_MODE_INPUT;
  gpioinit.Pull  = GPIO_PULLUP;
  gpioinit.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(FAULT_GATES_PORT, &gpioinit);

  gpioinit.Pin   = IMD_FAULT_GATE_PIN;
  gpioinit.Mode  = GPIO_MODE_INPUT;
  gpioinit.Pull  = GPIO_PULLUP;
  gpioinit.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(FAULT_GATES_PORT, &gpioinit);

  gpioinit.Pin   = SDN_FAULT_GATE_PIN;
  gpioinit.Mode  = GPIO_MODE_INPUT;
  gpioinit.Pull  = GPIO_PULLUP;
  gpioinit.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(FAULT_GATES_PORT, &gpioinit);

  gpioinit.Pin   = ESD_FAULT_PIN;
  gpioinit.Mode  = GPIO_MODE_INPUT;
  gpioinit.Pull  = GPIO_PULLUP;
  gpioinit.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(FAULT_GATES_PORT, &gpioinit);
}

void updateGateFaults() {
  // If we read a GPIO of on, we set the fault to true, otherwise set to false
  gate_faults.bms_fault =
    (HAL_GPIO_ReadPin(FAULT_GATES_PORT, BMS_FAULT_GATE_PIN) == GPIO_PIN_RESET);
  gate_faults.bpd_fault =
    (HAL_GPIO_ReadPin(FAULT_GATES_PORT, BPD_FAULT_GATE_PIN) == GPIO_PIN_RESET);
  gate_faults.imd_fault =
    (HAL_GPIO_ReadPin(FAULT_GATES_PORT, IMD_FAULT_GATE_PIN) == GPIO_PIN_RESET);
  gate_faults.sdn_fault =
    (HAL_GPIO_ReadPin(FAULT_GATES_PORT, SDN_FAULT_GATE_PIN) == GPIO_PIN_RESET);
  gate_faults.esd_fault =
    (HAL_GPIO_ReadPin(FAULT_GATES_PORT, ESD_FAULT_PIN) == GPIO_PIN_RESET);
}

bool anyGateFaultsTripped() {
  return gate_faults.bms_fault ||
         gate_faults.bpd_fault ||
         gate_faults.imd_fault ||
         gate_faults.sdn_fault ||
         gate_faults.esd_fault;
}

bool anyGateNonESDFaultsTripped() {
  return gate_faults.bms_fault ||
         gate_faults.bpd_fault ||
         gate_faults.imd_fault ||
         gate_faults.sdn_fault;
}

void printGateFaults() {
  if (anyGateFaultsTripped()) {
    printf("\r\n[ERROR]: THE FOLLOWING FAULT GATES WERE TRIPPED:\r\n");

    if (gate_faults.bms_fault) {
      printf("\t[FAULT]: BMS FAULT\r\n");
    }

    if (gate_faults.bpd_fault) {
      printf("\t[FAULT]: BPD FAULT\r\n");
    }

    if (gate_faults.imd_fault) {
      printf("\t[FAULT]: IMD FAULT\r\n");
    }

    if (gate_faults.sdn_fault) {
      printf("\t[FAULT]: SDN FAULT\r\n");
    }

    if (gate_faults.esd_fault) {
      printf("\t[FAULT]: ESD FAULT\r\n");
    }
  } else {
    printf("NO GATE FAULTS WERE TRIPPED.");
  }
}
