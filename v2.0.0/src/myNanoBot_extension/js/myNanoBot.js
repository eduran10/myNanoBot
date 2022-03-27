// myNanoBot.js (is based mBlock arduino.js/demo.js)
// This script/extension requires mynanobot_firmware. 
// Please upload it to your arduino control card.
// https://github.com/eduran10/mynanobot
 
(function(ext) {
	var device = null;
	var _rxBuf = [];

	var tones ={
		"C" : 2093, //C7
		"D" : 2349, //D7
		"E" : 2637, //E7
		"F" : 2794, //F7
		"G" : 3136, //G7
		"A" : 3520, //A7
		"B" : 3951 //B7
	};
	
	var beats = {
		"Zero" : 0,
		"Eighth" : 125,
		"Quarter" : 250,
		"Half" : 500,
		"Whole" : 1000,
		"Double" : 2000
	};

	var values = {};
	var indexs = [];
	var startTimer = 0;

	ext.resetAll = function(){
		device.send([0xff, 0x55, 2, 0, 4]);
	};

	ext.runArduino = function(){
		responseValue();
	};

	ext.runForward = function(speed)
	{
		runPackage(5,102,speed);
	};

	ext.runBackward = function(speed)
	{		
		runPackage(5,98,speed);
	};

	ext.turnRight = function(speed)
	{		
		runPackage(5,114,speed);	
	};

	ext.turnLeft = function(speed)
	{		
		runPackage(5,108,speed);	
	};

	ext.runRightMotor = function(dir,speed)
	{		
		if (dir == "forward") {	
		  runPackage(10,1,102,speed);
		} else if (dir == "backward") {
		  runPackage(10,1,98,speed);
		}
	};	

	ext.runLeftMotor = function(dir,speed)
	{		
		if (dir == "forward") {	
		  runPackage(10,3,102,speed);
		} else if (dir == "backward") {
		  runPackage(10,3,98,speed);
		}
	};

	ext.stopBot = function()
	{
		runPackage(5,115,0);  
	};

	ext.setRGBLed = function(color)
	{		
		switch(color) {
			case "Red":
			  runPackage(8,255,0,0);	
			  break;
			case "Green":
			  runPackage(8,0,255,0);	
			  break;
			case "Blue":
			  runPackage(8,0,0,255);	
			  break;
			default:
			  runPackage(8,0,0,0);	
			  break;			
		}	
	};

	ext.setRGBLedValue = function(r, g, b)
	{	
		runPackage(8,r,g,b);	
	};

	ext.getLDR = function(nextID)
	{
		getPackage(nextID, 29);
	}

	ext.getLDRValue = function(nextID)
	{
		getPackage(nextID, 3);
	}

	ext.getSoundSensor = function(nextID)
	{
		getPackage(nextID, 39);
	}
	
	ext.getSoundSensorValue = function(nextID)
	{
		getPackage(nextID, 7);
	}	

	ext.getHCSR04Value = function (nextID)
	{
		getPackage(nextID, 1);
	}

	ext.playTone = function (tone,beat){
		runPackage(34, short2array(tones[tone]),short2array(beats[beat]));
	}

	ext.getTimer = function(nextID){
		if(startTimer==0){
			startTimer = (new Date().getTime())/1000.0;
		}
		responseValue(nextID,(new Date().getTime())/1000.0-startTimer);
	}

	ext.resetTimer = function(){
		startTimer = (new Date().getTime())/1000.0;
		responseValue();
	};	

	/*******************************
	********************************
	*******************************/
	function sendPackage(argList, type){
		var bytes = [0xff, 0x55, 0, 0, type];
		for(var i=0;i<argList.length;++i){
			var val = argList[i];
			if(val.constructor == "[class Array]"){
				bytes = bytes.concat(val);
			}else{
				bytes.push(val);
			}
		}
		bytes[2] = bytes.length - 3;
		device.send(bytes);
	}
	
	function runPackage(){
		sendPackage(arguments, 2);
	}
	function getPackage(){
		var nextID = arguments[0];
		Array.prototype.shift.call(arguments);
		sendPackage(arguments, 1);
	}

    var inputArray = [];
	var _isParseStart = false;
	var _isParseStartIndex = 0;
    function processData(bytes) {
		var len = bytes.length;
		if(_rxBuf.length>30){
			_rxBuf = [];
		}
		for(var index=0;index<bytes.length;index++){
			var c = bytes[index];
			_rxBuf.push(c);
			if(_rxBuf.length>=2){
				if(_rxBuf[_rxBuf.length-1]==0x55 && _rxBuf[_rxBuf.length-2]==0xff){
					_isParseStart = true;
					_isParseStartIndex = _rxBuf.length-2;
				}
				if(_rxBuf[_rxBuf.length-1]==0xa && _rxBuf[_rxBuf.length-2]==0xd&&_isParseStart){
					_isParseStart = false;
					
					var position = _isParseStartIndex+2;
					var extId = _rxBuf[position];
					position++;
					var type = _rxBuf[position];
					position++;
					//1 byte 2 float 3 short 4 len+string 5 double
					var value;
					switch(type){
						case 1:{
							value = _rxBuf[position];
							position++;
						}
							break;
						case 2:{
							value = readFloat(_rxBuf,position);
							position+=4;
							if(value<-255||value>1023){
								value = 0;
							}
						}
							break;
						case 3:{
							value = readInt(_rxBuf,position,2);
							position+=2;
						}
							break;
						case 4:{
							var l = _rxBuf[position];
							position++;
							value = readString(_rxBuf,position,l);
						}
							break;
						case 5:{
							value = readDouble(_rxBuf,position);
							position+=4;
						}
							break;
						case 6:
							value = readInt(_rxBuf,position,4);
							position+=4;
							break;
					}
					if(type<=6){
						responseValue(extId,value);
					}else{
						responseValue();
					}
					_rxBuf = [];
				}
			} 
		}
    }
	function readFloat(arr,position){
		var f= [arr[position],arr[position+1],arr[position+2],arr[position+3]];
		return parseFloat(f);
	}
	function readInt(arr,position,count){
		var result = 0;
		for(var i=0; i<count; ++i){
			result |= arr[position+i] << (i << 3);
		}
		return result;
	}
	function readDouble(arr,position){
		return readFloat(arr,position);
	}
	function readString(arr,position,len){
		var value = "";
		for(var ii=0;ii<len;ii++){
			value += String.fromCharCode(_rxBuf[ii+position]);
		}
		return value;
	}
    function appendBuffer( buffer1, buffer2 ) {
        return buffer1.concat( buffer2 );
    }

    // Extension API interactions
    var potentialDevices = [];
    ext._deviceConnected = function(dev) {
        potentialDevices.push(dev);

        if (!device) {
            tryNextDevice();
        }
    }

    function tryNextDevice() {
        // If potentialDevices is empty, device will be undefined.
        // That will get us back here next time a device is connected.
        device = potentialDevices.shift();
        if (device) {
            device.open({ stopBits: 0, bitRate: 115200, ctsFlowControl: 0 }, deviceOpened);
        }
    }

    var watchdog = null;
    function deviceOpened(dev) {
        if (!dev) {
            // Opening the port failed.
            tryNextDevice();
            return;
        }
        device.set_receive_handler('myNanoBot',processData);
    };

    ext._deviceRemoved = function(dev) {
        if(device != dev) return;
        device = null;
    };

    ext._shutdown = function() {
        if(device) device.close();
        device = null;
    };

    ext._getStatus = function() {
        if(!device) return {status: 1, msg: 'myNanoBot disconnected'};
        if(watchdog) return {status: 1, msg: 'Probing for myNanoBot'};
        return {status: 2, msg: 'myNanoBot connected'};
    }

    var descriptor = {};
	ScratchExtensions.register('myNanoBot', descriptor, ext, {type: 'serial'});
})({});
