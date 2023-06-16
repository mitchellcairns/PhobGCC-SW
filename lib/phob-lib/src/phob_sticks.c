void phob_process_sticks()
{
  readADCScale(_ADCScale, _ADCScaleFactor);

	//on Arduino (and therefore Teensy), micros() overflows after about 71.58 minutes
	//This is 2^32 microseconds
	static uint32_t lastMicros = micros();
	static uint32_t adcDelta = 0;
	//We increment lastMicros by 1000 each timestep so that we can always try to catch up
	// to the right timestep instead of always being late
	//If the timestep goes past 2^32, then we subtract 2^32.
	//However, this may make it smaller than the most recently measured time.
	//So, we let the loop keep going if

//#ifndef CLEANADC
	//Read the sticks repeatedly until it's been 1 millisecond since the last iteration
	//This is for denoising and making sure the loop runs at 1000 Hz
	//We want to stop the ADC reading early enough that we don't overrun 1000 microseconds
	uint32_t adcCount = 0;
	uint32_t aXSum = 0;
	uint32_t aYSum = 0;
	uint32_t cXSum = 0;
	uint32_t cYSum = 0;
	uint32_t beforeMicros = micros();
	uint32_t afterMicros;
	do{
		adcCount++;
		aXSum += readAx(pin);
		aYSum += readAy(pin);
		cXSum += readCx(pin);
		cYSum += readCy(pin);
		afterMicros = micros();
		adcDelta = afterMicros-beforeMicros;
		beforeMicros = afterMicros;
	}
	while((afterMicros-lastMicros < 1000 - adcDelta) || (afterMicros-lastMicros > 100000000));

	//Then we spinlock to get the 1 kHz more exactly.
	while(afterMicros-lastMicros < 1000) {
		afterMicros = micros();
	}

	//debug_println(adcCount);
	float aStickX = aXSum/(float)adcCount/4096.0*_ADCScale;
	float aStickY = aYSum/(float)adcCount/4096.0*_ADCScale;
	float cStickX = cXSum/(float)adcCount/4096.0*_ADCScale;
	float cStickY = cYSum/(float)adcCount/4096.0*_ADCScale;
	/*
#else //CLEANADC: read only once
	float aStickX = readAx(pin)/4096.0;
	float aStickY = readAy(pin)/4096.0;
	float cStickX = readCx(pin)/4096.0;
	float cStickY = readCy(pin)/4096.0;
	//note: this actually results in about 0.5 ms delay for the analog sticks

	uint32_t thisMicros = micros();
	while(thisMicros-lastMicros < 1000) {
		thisMicros = micros();
	}
#endif //CLEANADC
	*/
	dT = (micros()-lastMicros)/1000;
	lastMicros += 1000;
	if(micros() - lastMicros > 1500) { //handle the case that it was synced and now isn't
		lastMicros = micros();
	}
	raw.axRaw = aStickX;
	raw.ayRaw = aStickY;
	raw.cxRaw = cStickX;
	raw.cyRaw = cStickY;

	//create the measurement value to be used in the kalman filter
	float xZ;
	float yZ;

	//linearize the analog stick inputs by multiplying by the coefficients found during calibration (3rd order fit)
	xZ = linearize(aStickX, aStickParams.fitCoeffsX);
	yZ = linearize(aStickY, aStickParams.fitCoeffsY);

	float posCx = linearize(cStickX, cStickParams.fitCoeffsX);
	float posCy = linearize(cStickY, cStickParams.fitCoeffsY);

	float posAx = xZ;
	float posAy = yZ;

	raw.axLinearized = posAx;
	raw.ayLinearized = posAy;
	raw.cxLinearized = posCx;
	raw.cyLinearized = posCy;

	//Run the kalman filter to eliminate snapback
	static float xPosFilt = 0;//output of kalman filter
	static float yPosFilt = 0;//output of kalman filter
	runKalman(xPosFilt, yPosFilt, xZ, yZ, controls, normGains);

	float shapedAx = xPosFilt;
	float shapedAy = yPosFilt;
	//Run waveshaping, a secondary filter to extend time at the rim
	aRunWaveShaping(shapedAx, shapedAy, shapedAx, shapedAy, controls, normGains);

	//Run a simple low-pass filter
	static float oldPosAx = 0;
	static float oldPosAy = 0;
	posAx = normGains.xSmoothing*shapedAx + (1-normGains.xSmoothing)*oldPosAx;
	posAy = normGains.ySmoothing*shapedAy + (1-normGains.ySmoothing)*oldPosAy;
	oldPosAx = posAx;
	oldPosAy = posAy;

	//Run waveshaping on the c-stick
	cRunWaveShaping(posCx, posCy, posCx, posCy, controls, normGains);

	//Run a simple low-pass filter on the C-stick
	static float cXPos = 0;
	static float cYPos = 0;
	float oldCX = cXPos;
	float oldCY = cYPos;
	cXPos = posCx;
	cYPos = posCy;
	float xWeight1 = normGains.cXSmoothing;
	float xWeight2 = 1-xWeight1;
	float yWeight1 = normGains.cYSmoothing;
	float yWeight2 = 1-yWeight1;

	cXPos = xWeight1*cXPos + xWeight2*oldCX;
	cYPos = yWeight1*cYPos + yWeight2*oldCY;

	posCx = cXPos;
	posCy = cYPos;

	//Run a median filter to reduce noise
#ifdef USEMEDIAN
	static float xPosList[MEDIANLEN] = MEDIANARRAY;//for median filtering;
	static float yPosList[MEDIANLEN] = MEDIANARRAY;//for median filtering
	static unsigned int xMedianIndex = 0;
	static unsigned int yMedianIndex = 0;
    runMedian(posAx, xPosList, xMedianIndex);
    runMedian(posAy, yPosList, yMedianIndex);
#endif

	float remappedAx;
	float remappedAy;
	float remappedCx;
	float remappedCy;
	float remappedAxUnfiltered;
	float remappedAyUnfiltered;
	float remappedCxUnfiltered;
	float remappedCyUnfiltered;
	notchRemap(posAx, posAy, &remappedAx, &remappedAy, _noOfNotches, aStickParams, currentCalStep, controls, ASTICK);
	notchRemap(posCx, posCy, &remappedCx, &remappedCy, _noOfNotches, cStickParams, currentCalStep, controls, CSTICK);
	notchRemap(raw.axLinearized, raw.ayLinearized, &remappedAxUnfiltered, &remappedAyUnfiltered, _noOfNotches, aStickParams, 1, controls, ASTICK);//no snapping
	notchRemap(raw.cxLinearized, raw.cyLinearized, &remappedCxUnfiltered, &remappedCyUnfiltered, _noOfNotches, cStickParams, 1, controls, CSTICK);//no snapping

	//Clamp values from -125 to +125
	remappedAx = fmin(125, fmax(-125, remappedAx));
	remappedAy = fmin(125, fmax(-125, remappedAy));
	remappedCx = fmin(125, fmax(-125, remappedCx));
	remappedCy = fmin(125, fmax(-125, remappedCy));
	raw.axUnfiltered = fmin(125, fmax(-125, remappedAxUnfiltered));
	raw.ayUnfiltered = fmin(125, fmax(-125, remappedAyUnfiltered));
	raw.cxUnfiltered = fmin(125, fmax(-125, remappedCxUnfiltered));
	raw.cyUnfiltered = fmin(125, fmax(-125, remappedCyUnfiltered));

	bool skipAHyst = false;
#ifdef EXTRAS_ESS
	//ESS adapter functionality for Ocarina of Time on WiiVC if enabled
	skipAHyst = ess::remap(&remappedAx, &remappedAy, controls.extras[ess::extrasEssConfigSlot].config);
#endif

	float hystVal = 0.3;
	//assign the remapped values to the button struct
	if(readA){
		if (!skipAHyst) {
			float diffAx = (remappedAx+_floatOrigin)-btn.Ax;
			if( (diffAx > (1.0 + hystVal)) || (diffAx < -hystVal) ){
				btn.Ax = (uint8_t) (remappedAx+_floatOrigin);
			}
			float diffAy = (remappedAy+_floatOrigin)-btn.Ay;
			if( (diffAy > (1.0 + hystVal)) || (diffAy < -hystVal) ){
				btn.Ay = (uint8_t) (remappedAy+_floatOrigin);
			}
		} else {
			btn.Ax = (uint8_t) (remappedAx+_floatOrigin);
			btn.Ay = (uint8_t) (remappedAy+_floatOrigin);
		}
	}
	if(readC){
		float diffCx = (remappedCx+_floatOrigin)-btn.Cx;
		if( (diffCx > (1.0 + hystVal)) || (diffCx < -hystVal) ){
			btn.Cx = (uint8_t) (remappedCx+_floatOrigin);
		}
		float diffCy = (remappedCy+_floatOrigin)-btn.Cy;
		if( (diffCy > (1.0 + hystVal)) || (diffCy < -hystVal) ){
			btn.Cy = (uint8_t) (remappedCy+_floatOrigin);
		}
	}
}
