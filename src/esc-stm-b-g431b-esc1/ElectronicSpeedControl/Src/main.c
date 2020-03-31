  /* USER CODE BEGIN 2 */
  
  /*Initialization of Electronic Speed Controller (STEVAL-ESC001V1)*/
  esc_boot(&ESC_M1);
  
  /* USER CODE END 2 */
  
     /* USER CODE BEGIN 3 */
    /* Main routine for Electronic Speed Controller (STEVAL-ESC001V1) */
    esc_pwm_control(&ESC_M1); 

  }
  /* USER CODE END 3 */ 