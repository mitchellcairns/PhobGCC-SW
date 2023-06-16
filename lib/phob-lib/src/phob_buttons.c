void phob_process_buttons()
{
  //Gather the button data from the hardware
	readButtons(pin, hardware);

	hardware.La = (uint8_t) readLa(pin, controls.lTrigInitial, 1);
	hardware.Ra = (uint8_t) readRa(pin, controls.rTrigInitial, 1);

	//Copy hardware buttons into a temp
	Buttons tempBtn;
	copyButtons(hardware, tempBtn);

	//Swap buttons here for jump remapping
	applyJump(controls, hardware, tempBtn);

	//read the L and R sliders here instead of readSticks so we don't get race conditions for mode 6

	//set up lockout for mode 5; it's not permissible to have analog trigger
	// inputs available while mode 5 is active
	//when a trigger is in lockout due to the other being mode 5,
	// modes 1, 3, and 4 will have no output on that trigger to warn the user.
	//(the above modes are 1-indexed, user-facing values)
	const bool lockoutL = controls.rConfig == 4 && (controls.lConfig != 1 && controls.lConfig != 4 && controls.lConfig != 5);
	const bool lockoutR = controls.lConfig == 4 && (controls.rConfig != 1 && controls.rConfig != 4 && controls.rConfig != 5);

	//We multiply the analog trigger reads by this to shut them off if the trigger is mapped to jump
	const int shutoffLa = (controls.jumpConfig == SWAP_XL || controls.jumpConfig == SWAP_YL) ? 0 : 1;
	const int shutoffRa = (controls.jumpConfig == SWAP_XR || controls.jumpConfig == SWAP_YR) ? 0 : 1;

	//These are used for mode 7, but they're calculated out here so we can scale the deadzone too.
	float triggerScaleL = (0.0112f * controls.lTriggerOffset) + 0.4494f;
	float triggerScaleR = (0.0112f * controls.rTriggerOffset) + 0.4494f;

	switch(controls.lConfig) {
		case 0: //Default Trigger state
			tempBtn.La = (uint8_t) readLa(pin, controls.lTrigInitial, 1) * shutoffLa;
			break;
		case 1: //Digital Only Trigger state
			tempBtn.La = (uint8_t) 0;
			break;
		case 2: //Analog Only Trigger state
			tempBtn.L  = (uint8_t) 0;
			tempBtn.La = (uint8_t) readLa(pin, controls.lTrigInitial, 1) * shutoffLa;
			break;
		case 3: //Trigger Plug Emulation state
			tempBtn.La = (uint8_t) fmin(controls.lTriggerOffset, readLa(pin, controls.lTrigInitial, 1) * shutoffLa);
			break;
		case 4: //Digital => Analog Value state
			if(tempBtn.L) {
				tempBtn.La = (uint8_t) min(controls.lTriggerOffset, 255);
			} else {
				tempBtn.La = (uint8_t) 0;
			}
			tempBtn.L = (uint8_t) 0;
			break;
		case 5: //Digital => Analog Value + Digital state
			if(tempBtn.L) {
				tempBtn.La = (uint8_t) min(controls.lTriggerOffset, 255);
			} else {
				tempBtn.La = (uint8_t) 0;
			}
			break;
		case 6: //Scales Analog Trigger Values
			tempBtn.La = (uint8_t) readLa(pin, controls.lTrigInitial, triggerScaleL) * shutoffLa;
			break;
		default:
			tempBtn.La = (uint8_t) readLa(pin, controls.lTrigInitial, 1) * shutoffLa;
	}
	if(lockoutL){
		tempBtn.L  = (uint8_t) 0;
		tempBtn.La = (uint8_t) 0;
	}

	switch(controls.rConfig) {
		case 0: //Default Trigger state
			tempBtn.Ra = (uint8_t) readRa(pin, controls.rTrigInitial, 1) * shutoffRa;
			break;
		case 1: //Digital Only Trigger state
			tempBtn.Ra = (uint8_t) 0;
			break;
		case 2: //Analog Only Trigger state
			tempBtn.R  = (uint8_t) 0;
			tempBtn.Ra = (uint8_t) readRa(pin, controls.rTrigInitial, 1) * shutoffRa;
			break;
		case 3: //Trigger Plug Emulation state
			tempBtn.Ra = (uint8_t) fmin(controls.rTriggerOffset, readRa(pin, controls.rTrigInitial, 1) * shutoffRa);
			break;
		case 4: //Digital => Analog Value state
			if(tempBtn.R) {
				tempBtn.Ra = (uint8_t) min(controls.rTriggerOffset, 255);
			} else {
				tempBtn.Ra = (uint8_t) 0;
			}
			tempBtn.R = (uint8_t) 0;
			break;
		case 5: //Digital => Analog Value + Digital state
			if(tempBtn.R) {
				tempBtn.Ra = (uint8_t) min(controls.rTriggerOffset, 255);
			} else {
				tempBtn.Ra = (uint8_t) 0;
			}
			break;
		case 6: //Scales Analog Trigger Values
			tempBtn.Ra = (uint8_t) readRa(pin, controls.rTrigInitial, triggerScaleR) * shutoffRa;
			break;
		default:
			tempBtn.Ra = (uint8_t) readRa(pin, controls.rTrigInitial, 1) * shutoffRa;
	}
	if(lockoutR){
		tempBtn.R  = (uint8_t) 0;
		tempBtn.Ra = (uint8_t) 0;
	}

	//Apply any further button remapping to tempBtn here

	//Tournament toggle
	static int startLockout = 1000;
	if(controls.tournamentToggle >= 3 && hardware.S) {
		if(startLockout > 0) {
			startLockout--;
			tempBtn.S = (uint8_t) (0);
		} else if(startLockout <= 0) {
			tempBtn.S = (uint8_t) (1);
		}
	} else if(startLockout < 1000) {
		startLockout++;
	}
	static int duLockout = 1000;
	if((controls.tournamentToggle == 1 || controls.tournamentToggle == 4) && hardware.Du) {
		if(duLockout > 0) {
			duLockout--;
			tempBtn.Du = (uint8_t) (0);
		} else if(duLockout <= 0) {
			tempBtn.Du = (uint8_t) (1);
		}
	} else if(duLockout < 1000) {
		duLockout++;
	}
	if(controls.tournamentToggle == 2 || controls.tournamentToggle == 5) {
		tempBtn.Du = (uint8_t) (0);
	}

	//Here we make sure LRAS actually operate.
	if(hardware.L && hardware.R && hardware.A && hardware.S) {
		tempBtn.L = (uint8_t) (1);
		tempBtn.R = (uint8_t) (1);
		tempBtn.A = (uint8_t) (1);
		tempBtn.S = (uint8_t) (1);
		tempBtn.La = (uint8_t) (255);
		tempBtn.Ra = (uint8_t) (255);
	}
	//Copy temp buttons (including analog triggers) back to btn
	copyButtons(tempBtn, btn);

	/* Current Commands List
	* Safe Mode:  AXY+Start
	* Display Version: AZ+Du
	*
	* Soft Reset:  ABZ+Start
	* Hard Reset:  ABZ+Dd
	* Auto-Initialize: AXY+Z
	* Tournament Toggle:  Z+Start
	*
	* Increase/Decrease Rumble: AB+Du/Dd
	* Show Current Rumble Setting: AB+Start
	*
	* Calibration
	* Analog Stick Calibration:  AXY+L
	* C-Stick Calibration:  AXY+R
	* Advance Calibration:  A or L or R
	* Undo Calibration:  Z
	* Skip to Notch Adjustment:  Start
	* Notch Adjustment CW/CCW:  X/Y
	* Notch Adjustment Reset:  B
	*
	* Analog Stick Configuration:
	* Increase/Decrease X-Axis Snapback Filtering:  AX+Du/Dd
	* Increase/Decrease Y-Axis Snapback Filtering:  AY+Du/Dd
	* Increase/Decrease X-Axis Waveshaping:  LX+Du/Dd
	* Increase/Decrease Y-Axis Waveshaping:  LY+Du/Dd
	* Increase/Decrease X-Axis Smoothing:  RX+Du/Dd
	* Increase/Decrease Y-Axis Smoothing:  RY+Du/Dd
	* Show Analog Filtering Settings: L+Start
	* Increase/Decrease Analog Scaler: LA+Du/Dd
	* Increase/Decrease Cardinal Snapping: RA+Du/Dd
	*
	* C-Stick Configuration
	* Increase/Decrease X-Axis Snapback Filtering:  AXZ+Du/Dd
	* Increase/Decrease Y-Axis Snapback Filtering:  AYZ+Du/Dd
	* Increase/Decrease X-Axis Waveshaping:  LXZ+Du/Dd
	* Increase/Decrease X-Axis Waveshaping:  LXZ+Du/Dd
	* Increase/Decrease X-Axis Offset:  RXZ+Du/Dd
	* Increase/Decrease Y-Axis Offset:  RYZ+Du/Dd
	* Show C-Stick Settings:  R+Start
	* Increase/Decrease Analog Scaler: LAZ+Du/Dd
	* Increase/Decrease Cardinal Snapping: RAZ+Du/Dd
	*
	* Swap X with Z:  XZ+Start
	* Swap Y with Z:  YZ+Start
	* Swap X with L:  LX+Start
	* Swap Y with L:  LY+Start
	* Swap X with R:  RX+Start
	* Swap Y with R:  Ry+Start
	*
	* Toggle L Trigger Mode:  AB+L
	* Toggle R Trigger Mode:  AB+R
	* Increase/Decrease L-trigger Offset:  LB+Du/Dd
	* Increase/Decrease R-Trigger Offset:  RB+Du/Dd
	*
	* Extras:
	* Toggle by holding both sticks in the chosen direction, then pressing A+B
	* Adjust by holding both sticks in the chosen direction, then pressing A+Dpad directions
	*/

	static bool advanceCal = false;

	//This will count up as we request settings changes continuously
	//If we enter the following if else block with a nonzero counter but no commands are used,
	// then that means we are done changing (for now) and can commit.
	//Primarily meant for the trigger offset setting, which has a lot of changes.
	static int settingChangeCount = 0;

	//check the hardware buttons to change the controller settings
	if(!controls.safeMode && (currentCalStep == -1)) {
		//it'll be unlocked after it hits zero
		const int hardResetLockoutDuration = 800;
		static int hardResetLockout = hardResetLockoutDuration;
		if(hardware.A && hardware.B && hardware.Z && hardware.Dd) { //Hard Reset pressed
			if(hardResetLockout > 0) { //Not held long enough
				hardResetLockout--;
			} else if(hardResetLockout == 0) { //Held long enough
				hardResetLockout = hardResetLockoutDuration;
				resetDefaults(HARD, controls, gains, normGains, _aStickParams, _cStickParams);//do reset sticks
				freezeSticks(2000, btn, hardware);
			}
		} else if(hardResetLockout < hardResetLockoutDuration) {
			hardResetLockout++;
		}

		if(hardware.A && hardware.X && hardware.Y && hardware.S && !hardware.L && !hardware.R) { //Safe Mode Toggle
			controls.safeMode = true;
			freezeSticks(4000, btn, hardware);
		} else if (hardware.A && hardware.Z && hardware.Du && !hardware.X && !hardware.Y && !hardware.L && !hardware.R) { //display version number (ignore commands for c stick snapback)
			const int versionHundreds = floor(SW_VERSION/100.0);
			const int versionOnes     = SW_VERSION-versionHundreds;
			btn.Ax = (uint8_t) _floatOrigin;
			btn.Ay = (uint8_t) _floatOrigin;
			btn.Cx = (uint8_t) _floatOrigin + versionHundreds;
			btn.Cy = (uint8_t) _floatOrigin + versionOnes;
			clearButtons(2000, btn, hardware);
		} else if (hardware.A && hardware.B && hardware.Z && hardware.S) { //Soft Reset
			resetDefaults(SOFT, controls, gains, normGains, _aStickParams, _cStickParams);//don't reset sticks
			freezeSticks(2000, btn, hardware);
		} else if (hardware.A && hardware.B && hardware.Z && hardware.Dd) { //Hard Reset
			//actually do nothing, this is just to prevent other things from happening
		} else if (hardware.A && hardware.X && hardware.Y && hardware.Z) { //Toggle Auto-Initialize
			settingChangeCount++;
			changeAutoInit(btn, hardware, controls);
		} else if(hardware.Z && hardware.S && !hardware.A && !hardware.B && !hardware.X && !hardware.Y) {
			settingChangeCount++;
			changeTournamentToggle(btn, hardware, controls);
		} else if (hardware.A && hardware.B && hardware.Du) { //Increase Rumble
			settingChangeCount++;
#ifdef RUMBLE
			changeRumble(INCREASE, btn, hardware, controls);
#else // RUMBLE
			//nothing
			freezeSticks(2000, btn, hardware);
#endif // RUMBLE
		} else if (hardware.A && hardware.B && hardware.Dd) { //Decrease Rumble
			settingChangeCount++;
#ifdef RUMBLE
			changeRumble(DECREASE, btn, hardware, controls);
#else // RUMBLE
			//nothing
			freezeSticks(2000, btn, hardware);
#endif // RUMBLE
		} else if (hardware.A && hardware.B && hardware.S) { //Show current rumble setting
			settingChangeCount++;
#ifdef RUMBLE
			showRumble(2000, btn, hardware, controls);
#else // RUMBLE
			freezeSticks(2000, btn, hardware);
#endif // RUMBLE
		} else if (hardware.A && hardware.X && hardware.Y && hardware.L) { //Analog Calibration
			debug_println("Calibrating the A stick");
			whichStick = ASTICK;
			currentCalStep ++;
			advanceCal = true;
			freezeSticks(2000, btn, hardware);
		} else if (hardware.A && hardware.X && hardware.Y && hardware.R) { //C-stick Calibration
			debug_println("Calibrating the C stick");
			whichStick = CSTICK;
			currentCalStep ++;
			advanceCal = true;
			freezeSticks(2000, btn, hardware);
		} else if(hardware.A && hardware.X && !hardware.Z && hardware.Du) { //Increase Analog X-Axis Snapback Filtering
			settingChangeCount++;
			adjustSnapback(XAXIS, INCREASE, btn, hardware, controls, gains, normGains);
		} else if(hardware.A && hardware.X && !hardware.Z && hardware.Dd) { //Decrease Analog X-Axis Snapback Filtering
			settingChangeCount++;
			adjustSnapback(XAXIS, DECREASE, btn, hardware, controls, gains, normGains);
		} else if(hardware.A && hardware.Y && !hardware.Z && hardware.Du) { //Increase Analog Y-Axis Snapback Filtering
			settingChangeCount++;
			adjustSnapback(YAXIS, INCREASE, btn, hardware, controls, gains, normGains);
		} else if(hardware.A && hardware.Y && !hardware.Z && hardware.Dd) { //Decrease Analog Y-Axis Snapback Filtering
			settingChangeCount++;
			adjustSnapback(YAXIS, DECREASE, btn, hardware, controls, gains, normGains);
		} else if(hardware.L && hardware.X && !hardware.Z && hardware.Du) { //Increase Analog X-Axis Waveshaping
			settingChangeCount++;
			adjustWaveshaping(ASTICK, XAXIS, INCREASE, btn, hardware, controls);
		} else if(hardware.L && hardware.X && !hardware.Z && hardware.Dd) { //Decrease Analog X-Axis Waveshaping
			settingChangeCount++;
			adjustWaveshaping(ASTICK, XAXIS, DECREASE, btn, hardware, controls);
		} else if(hardware.L && hardware.Y && !hardware.Z && hardware.Du) { //Increase Analog Y-Axis Waveshaping
			settingChangeCount++;
			adjustWaveshaping(ASTICK, YAXIS, INCREASE, btn, hardware, controls);
		} else if(hardware.L && hardware.Y && !hardware.Z && hardware.Dd) { //Decrease Analog Y-Axis Waveshaping
			settingChangeCount++;
			adjustWaveshaping(ASTICK, YAXIS, DECREASE, btn, hardware, controls);
		} else if(hardware.R && hardware.X && !hardware.Z && hardware.Du) { //Increase X-axis Delay
			settingChangeCount++;
			adjustSmoothing(XAXIS, INCREASE, btn, hardware, controls, gains, normGains);
		} else if(hardware.R && hardware.X && !hardware.Z && hardware.Dd) { //Decrease X-axis Delay
			settingChangeCount++;
			adjustSmoothing(XAXIS, DECREASE, btn, hardware, controls, gains, normGains);
		} else if(hardware.R && hardware.Y && !hardware.Z && hardware.Du) { //Increase Y-axis Delay
			settingChangeCount++;
			adjustSmoothing(YAXIS, INCREASE, btn, hardware, controls, gains, normGains);
		} else if(hardware.R && hardware.Y && !hardware.Z && hardware.Dd) { //Decrease Y-axis Delay
			settingChangeCount++;
			adjustSmoothing(YAXIS, DECREASE, btn, hardware, controls, gains, normGains);
		} else if(hardware.R && hardware.A && hardware.Du && !hardware.Z) { //Increase Cardinal Snapping
			settingChangeCount++;
			adjustCardinalSnapping(ASTICK, INCREASE, btn, hardware, controls);
		} else if(hardware.R && hardware.A && hardware.Dd && !hardware.Z) { //Decrease Cardinal Snapping
			settingChangeCount++;
			adjustCardinalSnapping(ASTICK, DECREASE, btn, hardware, controls);
		} else if(hardware.L && hardware.A && hardware.Du && !hardware.Z) { //Increase Analog Scaler
			settingChangeCount++;
			adjustAnalogScaler(ASTICK, INCREASE, btn, hardware, controls);
		} else if(hardware.L && hardware.A && hardware.Dd && !hardware.Z) { //Decrease Analog Scaler
			settingChangeCount++;
			adjustAnalogScaler(ASTICK, DECREASE, btn, hardware, controls);
		} else if(hardware.L && hardware.S && !hardware.A && !hardware.R && !hardware.X && !hardware.Y) { //Show Current Analog Settings (ignore L jump and L trigger toggle and LRAS)
			showAstickSettings(btn, hardware, controls, gains);
		} else if(hardware.A && hardware.X && hardware.Z && hardware.Du) { //Increase C-stick X-Axis Snapback Filtering
			settingChangeCount++;
			adjustCstickSmoothing(XAXIS, INCREASE, btn, hardware, controls, gains, normGains);
		} else if(hardware.A && hardware.X && hardware.Z && hardware.Dd) { //Decrease C-stick X-Axis Snapback Filtering
			settingChangeCount++;
			adjustCstickSmoothing(XAXIS, DECREASE, btn, hardware, controls, gains, normGains);
		} else if(hardware.A && hardware.Y && hardware.Z && hardware.Du) { //Increase C-stick Y-Axis Snapback Filtering
			settingChangeCount++;
			adjustCstickSmoothing(YAXIS, INCREASE, btn, hardware, controls, gains, normGains);
		} else if(hardware.A && hardware.Y && hardware.Z && hardware.Dd) { //Decrease C-stick Y-Axis Snapback Filtering
			settingChangeCount++;
			adjustCstickSmoothing(YAXIS, DECREASE, btn, hardware, controls, gains, normGains);
		} else if(hardware.L && hardware.X && hardware.Z && hardware.Du) { //Increase C-stick X-Axis Waveshaping
			settingChangeCount++;
			adjustWaveshaping(CSTICK, XAXIS, INCREASE, btn, hardware, controls);
		} else if(hardware.L && hardware.X && hardware.Z && hardware.Dd) { //Decrease C-stick X-Axis Waveshaping
			settingChangeCount++;
			adjustWaveshaping(CSTICK, XAXIS, DECREASE, btn, hardware, controls);
		} else if(hardware.L && hardware.Y && hardware.Z && hardware.Du) { //Increase C-stick Y-Axis Waveshaping
			settingChangeCount++;
			adjustWaveshaping(CSTICK, YAXIS, INCREASE, btn, hardware, controls);
		} else if(hardware.L && hardware.Y && hardware.Z && hardware.Dd) { //Decrease C-stick Y-Axis Waveshaping
			adjustWaveshaping(CSTICK, YAXIS, DECREASE, btn, hardware, controls);
		} else if(hardware.R && hardware.X && hardware.Z && hardware.Du) { //Increase C-stick X Offset
			settingChangeCount++;
			adjustCstickOffset(XAXIS, INCREASE, btn, hardware, controls);
		} else if(hardware.R && hardware.X && hardware.Z && hardware.Dd) { //Decrease C-stick X Offset
			settingChangeCount++;
			adjustCstickOffset(XAXIS, DECREASE, btn, hardware, controls);
		} else if(hardware.R && hardware.Y && hardware.Z && hardware.Du) { //Increase C-stick Y Offset
			settingChangeCount++;
			adjustCstickOffset(YAXIS, INCREASE, btn, hardware, controls);
		} else if(hardware.R && hardware.Y && hardware.Z && hardware.Dd) { //Decrease C-stick Y Offset
			settingChangeCount++;
			adjustCstickOffset(YAXIS, DECREASE, btn, hardware, controls);
		} else if(hardware.R && hardware.A && hardware.Z && hardware.Du) { //Increase C-stick Cardinal Snapping
			settingChangeCount++;
			adjustCardinalSnapping(CSTICK, INCREASE, btn, hardware, controls);
		} else if(hardware.R && hardware.A && hardware.Z && hardware.Dd) { //Decrease C-stick Cardinal Snapping
			settingChangeCount++;
			adjustCardinalSnapping(CSTICK, DECREASE, btn, hardware, controls);
		} else if(hardware.L && hardware.A && hardware.Z && hardware.Du) { //Increase C-stick Analog Scaler
			settingChangeCount++;
			adjustAnalogScaler(CSTICK, INCREASE, btn, hardware, controls);
		} else if(hardware.L && hardware.A && hardware.Z && hardware.Dd) { //Decrease C-stick Analog Scaler
			settingChangeCount++;
			adjustAnalogScaler(CSTICK, DECREASE, btn, hardware, controls);
		} else if(hardware.R && hardware.S && !hardware.A && !hardware.L && !hardware.X && !hardware.Y) { //Show Current C-stick Settings (ignore R jump and R trigger toggle and LRAS)
			showCstickSettings(btn, hardware, controls, gains);
		} else if(hardware.A && hardware.B && hardware.L) { //Toggle Analog L
			settingChangeCount++;
			nextTriggerState(LTRIGGER, btn, hardware, controls);
		} else if(hardware.A && hardware.B && hardware.R) { //Toggle Analog R
			settingChangeCount++;
			nextTriggerState(RTRIGGER, btn, hardware, controls);
		} else if(hardware.L && hardware.B && hardware.Du) { //Increase L-Trigger Offset
			settingChangeCount++;
			adjustTriggerOffset(LTRIGGER, INCREASE, btn, hardware, controls);
		} else if(hardware.L && hardware.B && hardware.Dd) { //Decrease L-trigger Offset
			settingChangeCount++;
			adjustTriggerOffset(LTRIGGER, DECREASE, btn, hardware, controls);
		} else if(hardware.R && hardware.B && hardware.Du) { //Increase R-trigger Offset
			settingChangeCount++;
			adjustTriggerOffset(RTRIGGER, INCREASE, btn, hardware, controls);
		} else if(hardware.R && hardware.B && hardware.Dd) { //Decrease R-trigger Offset
			settingChangeCount++;
			adjustTriggerOffset(RTRIGGER, DECREASE, btn, hardware, controls);
		} else if(hardware.X && hardware.Z && hardware.S) { //Swap X and Z
			settingChangeCount++;
			setJumpConfig(SWAP_XZ, controls);
			freezeSticks(2000, btn, hardware);
		} else if(hardware.Y && hardware.Z && hardware.S) { //Swap Y and Z
			settingChangeCount++;
			setJumpConfig(SWAP_YZ, controls);
			freezeSticks(2000, btn, hardware);
		} else if(hardware.X && hardware.L && hardware.S) { //Swap X and L
			settingChangeCount++;
			setJumpConfig(SWAP_XL, controls);
			freezeSticks(2000, btn, hardware);
		} else if(hardware.Y && hardware.L && hardware.S) { //Swap Y and L
			settingChangeCount++;
			setJumpConfig(SWAP_YL, controls);
			freezeSticks(2000, btn, hardware);
		} else if(hardware.X && hardware.R && hardware.S) { //Swap X and R
			settingChangeCount++;
			setJumpConfig(SWAP_XR, controls);
			freezeSticks(2000, btn, hardware);
		} else if(hardware.Y && hardware.R && hardware.S) { //Swap Y and R
			settingChangeCount++;
			setJumpConfig(SWAP_YR, controls);
			freezeSticks(2000, btn, hardware);
		} else if(checkAdjustExtra(EXTRAS_UP, btn, false)) { // Toggle Extras
			settingChangeCount++;
			toggleExtra(EXTRAS_UP, btn, hardware, controls);
		} else if(checkAdjustExtra(EXTRAS_DOWN, btn, false)) {
			settingChangeCount++;
			toggleExtra(EXTRAS_DOWN, btn, hardware, controls);
		} else if(checkAdjustExtra(EXTRAS_LEFT, btn, false)) {
			settingChangeCount++;
			toggleExtra(EXTRAS_LEFT, btn, hardware, controls);
		} else if(checkAdjustExtra(EXTRAS_RIGHT, btn, false)) {
			settingChangeCount++;
			toggleExtra(EXTRAS_RIGHT, btn, hardware, controls);
		} else if(checkAdjustExtra(EXTRAS_UP, btn, true)) { // Configure Extras
			settingChangeCount++;
			configExtra(EXTRAS_UP, btn, hardware, controls);
		} else if(checkAdjustExtra(EXTRAS_DOWN, btn, true)) {
			settingChangeCount++;
			configExtra(EXTRAS_DOWN, btn, hardware, controls);
		} else if(checkAdjustExtra(EXTRAS_LEFT, btn, true)) {
			settingChangeCount++;
			configExtra(EXTRAS_LEFT, btn, hardware, controls);
		} else if(checkAdjustExtra(EXTRAS_RIGHT, btn, true)) {
			settingChangeCount++;
			configExtra(EXTRAS_RIGHT, btn, hardware, controls);
		} else {
			//If the buttons were released after changing an applicable setting
			if(settingChangeCount > 0) {
				settingChangeCount = 0;
				//request a commit only if we need to batch them.
#ifdef BATCHSETTINGS
				commitSettings();
#endif //BATCHSETTINGS
			}
		}
	} else if (currentCalStep == -1) { //Safe Mode Enabled, Lock Settings, wait for safe mode command

		//it'll be unlocked after it hits zero
		const int safeModeLockoutDuration = 800;
		static int safeModeLockout = safeModeLockoutDuration;
		if(hardware.A && hardware.X && hardware.Y && hardware.S && !hardware.L && !hardware.R) { //Safe Mode toggle
			if(safeModeLockout > 0) { //Not held long enough
				safeModeLockout--;
			} else if(safeModeLockout == 0) { //Held long enough
				safeModeLockout = safeModeLockoutDuration;
				if(!running) { //wake it up if not already running
					running = true;
				}
				controls.safeMode = false;
				freezeSticks(2000, btn, hardware);
			}
		} else if(safeModeLockout < safeModeLockoutDuration) {
			safeModeLockout++;
		}
	}

	//Skip stick measurement and go to notch adjust using the start button while calibrating
	if(hardware.S && (currentCalStep >= 0 && currentCalStep < 32)){
		calibrationSkipMeasurement(currentCalStep, whichStick, tempCalPointsX, tempCalPointsY, notchStatus, notchAngles, measuredNotchAngles, aStickParams, cStickParams, controls);
	}

	//Undo Calibration using Z-button
	static bool undoCal = false;
	static bool undoCalPressed = false;
	if(hardware.Z && undoCal && !undoCalPressed) {
		undoCalPressed = true;
		calibrationUndo(currentCalStep, whichStick, notchStatus);
	} else if(!hardware.Z) {
		undoCalPressed = false;
	}

	//Advance Calibration Using L or R triggers or A button
	static int calibLockout = 50;
	static bool advanceCalPressed = false;
	if((hardware.A || hardware.L || hardware.R) && advanceCal){
		if(calibLockout > 0) {
			calibLockout--;
		} else if(calibLockout == 0 && !advanceCalPressed) {
			calibLockout = 50;
			advanceCalPressed = true;
			calibrationAdvance(controls, currentCalStep, whichStick, tempCalPointsX, tempCalPointsY, undoCal, notchAngles, notchStatus, measuredNotchAngles, aStickParams, cStickParams);
			if(currentCalStep == -1) {
				advanceCal = false;
			}
		}
	} else if(calibLockout < 50) {
		calibLockout++;
		advanceCalPressed = false;
	}
}
