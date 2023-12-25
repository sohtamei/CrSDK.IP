document.addEventListener('DOMContentLoaded', function (event) {
	let baseHost = 'localhost';
	let wsUrl1     = 'ws://' + baseHost + ':8080';
	let wsUrl2     = 'ws://' + baseHost + ':8082';
	let streamUrl1 = 'http://' + baseHost + ':81';
	let streamUrl2 = 'http://' + baseHost + ':82';
	let ws1 = null;
	let ws2 = null;
	let wsResolve = null;
	let wsWaitObj = null;

	const imgCanvas = document.getElementById('imgCanvas');
	const imgContext = imgCanvas.getContext('2d');

	const focusCanvas = document.getElementById('focusCanvas');
	const focusContext = focusCanvas.getContext('2d');

	const labelOutput = document.getElementById('labelOutput');
	const imgStream1 = document.getElementById('imgStream1');
	const imgStream2 = document.getElementById('imgStream2');

	imgStream1.onload = () => {
		imgCanvas.width  = imgStream1.width;
		imgCanvas.height = imgStream1.height;
		focusCanvas.height = imgStream1.height;
		//console.log(imgCanvas);
	};

	/*
	const prop = { FNumber: document.getElementById('FNumber'),
				 ShutterSpeed: document.getElementById('ShutterSpeed'),
				 IsoSensitivity: document.getElementById('IsoSensitivity'),
				 };
*/
	checkResp = function(type, resp) {
		if(wsResolve) {
		//	console.log(wsWaitObj);
		//	console.log(resp);
			if(wsWaitObj && wsWaitObj.hasOwnProperty('type')) {
				if(type != wsWaitObj.type)
					return;
				switch(type) {
				case 'json':
					if(wsWaitObj.hasOwnProperty('code')) {
						if(!resp.hasOwnProperty('code') || resp.code != wsWaitObj.code)
							return;
					}
					break;
				case 'object':
					break;
				}
			}
			wsResolve(resp);
			wsResolve = null;
			wsWaitObj = null;
		}
	}

	$(document).ready(function () {
		ws1 = new WebSocket(wsUrl1);

		ws1.onopen = function(event) {
			console.log('connected(1)');
			labelOutput.innerHTML = 'connected';

			return SendRecv1({cmd:'LiveView_Image_Quality',ope:'set',text:'High'})
			.then(resp => loadProps());
		}

		ws1.onerror = function(error) {
			console.log('ws1 error');
			ws1.close();
			ws1 = null;
		}

		ws1.onmessage = function(event) {
			let resp;
			//	console.log(event.data);
			let type = typeof(event.data);
			if(type == 'object') {
				const reader = new FileReader();
				reader.onload = () => { 
					stopStream(true);
					imgStream1.src = reader.result;
				}
				reader.readAsDataURL(event.data);
			} else if(type == 'string') {
				resp = JSON.parse(event.data);
				if(resp.code != "FocusFrameInfo") {
					console.log('R:'+event.data);	// debug
				}
				type = 'json';
				if(resp.hasOwnProperty('current') && resp.current.hasOwnProperty('text')) {
					resp.current.value = resp.current.text;
				}
				//if(prop.hasOwnProperty(resp.code)) {
				//  updateValue(prop[resp.code], resp.current.text, false);
				//}
			}

			checkResp(type, resp);

			if(type == 'json' && resp.hasOwnProperty('code') && isStreaming()) {
				// for frame info
				const info = resp.info;
				switch(resp.code) {
				case 'FaceFrameInfo':
				case 'TrackingFrameInfo':
			//	case 'Magnifier_Position':
				case 'FocusFrameInfo':
					imgContext.clearRect(0, 0, imgCanvas.width, imgCanvas.height);
					imgContext.strokeStyle = 'lime';

					for(let i = 0; i < info.length; i++) {
						let x = info[i][6]*1;
						let y = info[i][7]*1;
						let w = info[i][2]*1;
						let h = info[i][3]*1;
						x = x - w/2;
						y = y - h/2;
						x = (x/640)*imgCanvas.width;
						w = (w/640)*imgCanvas.width;
						y = (y/480)*imgCanvas.height;
						h = (h/480)*imgCanvas.height;
						imgContext.strokeRect(x,y,w,h);
					}
					break;
				case 'FollowFocusPositionCurrentValue':
					focusContext.clearRect(0, 0, focusCanvas.width, focusCanvas.height);
					focusContext.strokeStyle = 'lime';
					focusContext.strokeRect(0, (1.0 - resp.current.value/0xFFFF)*focusCanvas.height, focusCanvas.width, 1);
					break;
				}
			}
		}

		ws2 = new WebSocket(wsUrl2);

		ws2.onopen = function(event) {
			console.log('connected(2)');

			const json = JSON.stringify({cmd:'LiveView_Image_Quality',ope:'set',text:'High'});
			console.log('W:'+json);
			ws2.send(json);
		}

		ws2.onerror = function(error) {
			console.log('ws2 error');
			ws2.close();
			ws2 = null;
		}

		ws2.onmessage = function(event) {
			if(typeof(event.data) == 'object') {
				const reader = new FileReader();
				reader.onload = () => { 
					imgStream2.src = reader.result;
				}
				reader.readAsDataURL(event.data);
			}
		}
	});

	const buttonStream = document.getElementById('buttonStream');
	buttonStream.onclick = () => {
		if (!isStreaming()) {
			return startStream();
		} else {
			return stopStream();
		}
	}

	const startStream = () => {
		buttonStream.className = 'buttonOn';
		imgStream1.src = streamUrl1;
		imgStream2.src = streamUrl2;
	}

	const stopStream = (keepSrc = false) => {
		buttonStream.className = 'button';
		buttonS1.className = 'button';
		if(!keepSrc) {
			imgStream1.src = '';
			imgStream2.src = '';
		//	window.stop();
		}
		imgContext.clearRect(0, 0, imgCanvas.width, imgCanvas.height);
		return SendRecv12({cmd:'S1',ope:'set',text:'Unlocked'});
	}

	const isStreaming = () => {
		return (buttonStream.className == 'buttonOn');
	}

	const buttonS1 = document.getElementById('buttonS1');
	buttonS1.onclick = () => {
		if(buttonS1.className != 'buttonOn') {
			buttonS1.className = 'buttonOn';
			startStream();
			return SendRecv12({cmd:'S1',ope:'set',text:'Locked'});
		} else {
			buttonS1.className = 'button';
			imgContext.clearRect(0, 0, imgCanvas.width, imgCanvas.height);
			return SendRecv12({cmd:'S1',ope:'set',text:'Unlocked'});
		}
	}

	const buttonShutter = document.getElementById('buttonShutter');
	buttonShutter.onclick = () => {
		imgStream1.src = '';
		imgStream2.src = '';
	//	window.stop();
		return SendRecv12({cmd:'afShutter'});
	}

	const buttonRec = document.getElementById('buttonRec');
	buttonRec.onclick = () => {
		if(buttonRec.className != 'buttonOn') {
			buttonRec.className = 'buttonOn';

			return SendRecv12({cmd:'MovieRecord',ope:'Down'});
		} else {
			buttonRec.className = 'button';
			imgContext.clearRect(0, 0, imgCanvas.width, imgCanvas.height);
			return SendRecv12({cmd:'MovieRecord',ope:'Up'});
		}
	}

	const buttonUpdate = document.getElementById('buttonUpdate');
	buttonUpdate.onclick = () => {
		return loadProps();
	}

	const buttonDisconnect = document.getElementById('buttonDisconnect');
	buttonDisconnect.onclick = () => {
		return stopStream()
		.then(() => {
			ws1.close();  ws1 = null;
			ws2.close();  ws2 = null;
			labelOutput.innerHTML = 'disconnected';
			console.log('disconnected');
		});
	}

	imgCanvas.addEventListener('mousedown', (event) => {
	//	console.log(event);
		if(isStreaming()) {
			let x = event.offsetX;
			let y = event.offsetY;
			console.log(x+','+y);
			x = Math.trunc((x/imgCanvas.width)*640);
			y = Math.trunc((y/imgCanvas.height)*480);
			return SendRecv1({cmd:'RemoteTouchOperation',ope:'set',value:(x<<16)|y});
		}
	});

	const followFocusPos = (value) => {
		if(!isStreaming()) return;
		console.log(value);
		if(value < 0 || value > imgCanvas.height) return;
		value = Math.trunc((1.0 - value/imgCanvas.height)*0xFFFF);
		return SendRecv1({cmd:'FollowFocusPositionSetting',ope:'set',value:value});
	}

	focusCanvas.addEventListener('mousedown', (event) => {
		const startY = event.clientY;
		const startValue = event.offsetY;

		const onMouseMove = (event) => {
			followFocusPos(startValue + (event.clientY - startY));
		}

		const onMouseUp = event => {
			document.removeEventListener('mousemove', onMouseMove);
			document.removeEventListener('mouseup', onMouseUp);
		}
		document.addEventListener('mousemove', onMouseMove);
		document.addEventListener('mouseup', onMouseUp);
		followFocusPos(startValue);
	});


	const loadProps = () => {
		return SendRecv1({cmd:'getPropList'})
		.then(resp => {
			//console.log(resp);
			let i = 0;
			let _html = "";

			return new Promise((resolve,reject) => {
				loop();
				function loop(){
					const prop = resp.propList[i];
					return SendRecv1({cmd:prop, ope:'info'}, {type:'json', code:prop})
					.then(resp2 => {
						//console.log(resp2);
						const current = (resp2.current.hasOwnProperty('text')) ? resp2.current.text : resp2.current.value;
						if(resp2.hasOwnProperty('list')) {
							_html += '<div class="input-group"><label>' + prop + '</label><select id="' + prop + '" class="default-action">\n';
							for(let j = 0; j < resp2.list.length; j++) {
								const value = (resp2.list[j].hasOwnProperty('text')) ? resp2.list[j].text : resp2.list[j].value;
								if(value == current) {
									_html += '<option value="' + value + '" selected>' + value + '</option>\n'
								} else {
									_html += '<option value="' + value + '">' + value + '</option>\n'
								}
							}
							_html += '</select></div>';
						} else {
							_html += '<div class="input-group"><label>' + prop + '</label>';
							_html += '<div class="text"><input id="text' + prop + '" type="text" minlength="1" maxlength="20" size="20" value="' + current + '"></div>';
							if(resp2.incrementable != 'none') {
								_html += '<button id="' + prop + '" class="inline-button">Set</button>';
							}
							_html += '</div>';
						}
						i++;
						if(i >= resp.propList.length) {
							//console.log(_html);
							const divProp = document.getElementById('prop');
							divProp.innerHTML = _html;
							document.querySelectorAll('.default-action').forEach(el => {
								el.onchange = () => updateConfig(el);
							});
							document.querySelectorAll('.inline-button').forEach(el => {
								el.onclick = () => updateConfig(el);
							});
							resolve();
						} else {
							loop();
						}
					});
				}  // loop()
			}).then(() => {
				console.log('OK');
			});
		});
	}

	// Property (FNumber, ShutterSpeed, ..)
	document.querySelectorAll('.default-action').forEach(el => {
		el.onchange = () => updateConfig(el);
	})
/*
	const updateValue = (el, value, updateRemote) => {
console.log(el);
		updateRemote = (updateRemote == null) ? true : updateRemote;
		let initialValue;
		if (el.type === 'checkbox') {
			initialValue = el.checked;
			value = !!value;
			el.checked = value;
		} else {
			initialValue = el.value;
			el.value = value;
		}
	}
*/
	function updateConfig(el) {
		let value;
		switch (el.type) {
			case 'checkbox':
				value = el.checked ? 1 : 0;
				break;
			case 'range':
			case 'select-one':
				value = el.value;
				break;
			case 'button':
			case 'submit': {
				const textEl = document.getElementById('text'+el.id);
				if(!textEl) return;
				value = textEl.value;
				break;
			}
			default:
				return;
		}
		let sendObj = {cmd:el.id, ope:'set'};
		if(isNaN(value))
			sendObj['text'] = value.trim();
		else
			sendObj['value'] = Number(value);
		return SendRecv12(sendObj);
	}

	const hide = el => {
		el.classList.add('hidden');
	}

	const show = el => {
		el.classList.remove('hidden');
	}

	const disable = el => {
		el.classList.add('disabled');
		el.disabled = true
	}

	const enable = el => {
		el.classList.remove('disabled');
		el.disabled = false;
	}

	function SendRecv1(sendObj, waitObj=null) {
		if(!ws1) return 'error';

		return new Promise(resolve => {
			wsResolve = resolve;
			wsWaitObj = waitObj;
			const json = JSON.stringify(sendObj);
			console.log('W:'+json);
			ws1.send(json);
		}).then(result => {
			return result;
		});
	}

	function SendRecv12(sendObj, waitObj=null) {
		if(!ws1) return 'error';

		return new Promise(resolve => {
			wsResolve = resolve;
			wsWaitObj = waitObj;
			const json = JSON.stringify(sendObj);
			console.log('W:'+json);
			ws1.send(json);
			if(ws2) ws2.send(json);
		}).then(result => {
			return result;
		});
	}

})
