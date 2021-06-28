const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
	<meta http-equiv="Content-Language" content="pt-br">
	<head>
		<title>ESP Web Server</title>
		<meta name="viewport" content="width=device-width, initial-scale=1">
		<link rel="icon" href="data:,">
		<style>
			html
			{
				font-family: Arial, Helvetica, sans-serif;
				text-align: center;
			}
			h1
			{
				font-size: 1.8rem;
				color: white;
			}
			h2
			{
				font-size: 1.5rem;
				font-weight: bold;
				color: #143642;
			}
			.topnav
			{
				overflow: hidden;
				background-color: #143642;
			}
			body
			{
				margin: 0;
			}
			.content
			{
				padding: 30px;
				max-width: 600px;
				margin: 0 auto;
				display: flex;
				justify-content: center;
			}
			.card
			{
				background-color: #F8F7F9;;
				box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
				padding-top: 10px;
				padding-bottom: 10px;
				margin:10px;
				width: 500px;
			}
			.card2
			{
				background-color: #F8F7F9;;
				box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
				padding-top: 10px;
				padding-bottom: 10px;
				margin:10px;
				width: 600px;
			}
			.button
			{
				padding: 15px 50px;
				font-size: 24px;
				text-align: center;
				outline: none;
				color: #fff;
				background-color: #0f8b8d;
				border: none;
				border-radius: 5px;
				-webkit-touch-callout: none;
				-webkit-user-select: none;
				-khtml-user-select: none;
				-moz-user-select: none;
				-ms-user-select: none;
				user-select: none;
				-webkit-tap-highlight-color: rgba(0,0,0,0);
			}
			.button:active
			{
				background-color: #0f8b8d;
				box-shadow: 2 2px #CDCDCD;
				transform: translateY(2px);
			}
			.state
			{
				font-size: 1.5rem;
				color:#8c8c8c;
				font-weight: bold;
			}
			.info
			{
				font-size: 1rem;
				color:#8c8c8c;
			}
		</style>
		<title>ESP Web Server</title>
		<meta name="viewport" content="width=device-width, initial-scale=1">
		<link rel="icon" href="data:,">
	</head>
	<body>
		<div class="topnav">
			<h1>ESP WebSocket Server</h1>
		</div>
		<div class="content">
			<div class="card">
				<h2>Output - GPIO</h2>
				<p class="state">Led1: <span id="led1">%LED1%</span></p>
				<p><button id="buttonLed1" class="button">Toggle</button></p>
			</div>
			<div class="card">
				<h2>Output - GPIO</h2>
				<p class="state">Led2: <span id="led2">%LED2%</span></p>
				<p><button id="buttonLed2" class="button">Toggle</button></p>
			</div>
		</div>
		<div class="content">
			<div class="card2">
				<h2>Analogic Mesurement</h2>
				<p class="state">LDR: <span id="sensor">0</span></p>
			</div>
		</div>
		<div class="content">
			<div class="card2">
				<p class="info">FW Version: <span id="version">%VERSION%</span></p>
				<p class="info">HORA: <span id="time">%TIME%</span></p>
				<p class="info">RSSI: <span id="rssi">%RSSI%</span></p>
				<p class="info">IP: <span id="ip">%IP%</span></p>
				<p class="info">MAC: <span id="mac">%MAC%</span></p>
			</div>
		</div>
		<script>
			var gateway = `ws://${window.location.hostname}/ws`;
			var websocket;
			window.addEventListener('load', onLoad);
			function initWebSocket()
			{
				console.log('Trying to open a WebSocket connection...');
				websocket = new WebSocket(gateway);
				websocket.onopen = onOpen;
				websocket.onclose = onClose;
				websocket.onmessage = onMessage;
			}
			function onOpen(event)
			{
				console.log('Connection opened');
			}
			function onClose(event)
			{
				console.log('Connection closed');
				setTimeout(initWebSocket, 2000);
			}
			function onMessage(event)
			{	
				var data = JSON.parse(event.data);
				
				if(data.hasOwnProperty('led1'))
					document.getElementById('led1').innerHTML = data.led1 == "1" ? "ON" : "OFF";
				if(data.hasOwnProperty('led2'))
					document.getElementById('led2').innerHTML = data.led2 == "1" ? "ON" : "OFF";
				if(data.hasOwnProperty('time'))
					document.getElementById('time').innerHTML = data.time;
				if(data.hasOwnProperty('sensor'))
					document.getElementById('sensor').innerHTML = data.sensor;
			}
			function onLoad(event)
			{
				initWebSocket();
				initButton();      
			}
			function initButton()
			{
				document.getElementById('buttonLed1').addEventListener('click', toggleLed1);
				document.getElementById('buttonLed2').addEventListener('click', toggleLed2);
			}
			function toggleLed1()
			{
				websocket.send('toggleLed1');
			}
			function toggleLed2()
			{
				websocket.send('toggleLed2');
			}
		</script>
	</body>
</html>
)rawliteral";