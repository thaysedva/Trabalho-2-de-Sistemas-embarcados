const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
	<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
	<meta http-equiv="Content-Language" content="pt-br">
	<script src="https://code.highcharts.com/highcharts.js"></script>
	<head>
		<title>Trabalho 2 - Sistemas Embarcados</title>
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
			.container
			{
				display: inline-block;
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
		<title>Trabalho 2 - Sistemas Embarcados</title>
		<meta name="viewport" content="width=device-width, initial-scale=1">
		<link rel="icon" href="data:,">
	</head>
	<body>
		<div class="topnav">
			<h1>Trabalho 2 - Sistemas Embarcados</h1>
		</div>
		<div class="content">
			<div class="card">
				<h2>LED1</h2>
				<img id="icon1" src="http://everaldoprojects.hol.es/LED-OFF.png" height="100px"/>
				<p class="state"><span id="led1">%LED1%</span></p>
				<p><button id="buttonLed1" class="button">Toggle</button></p>
			</div>
			<div class="card">
				<h2>LED2</h2>
				<img id="icon2" src="http://everaldoprojects.hol.es/LED-OFF.png" height="100px"/>
				<p class="state"><span id="led2">%LED2%</span></p>
				<p><button id="buttonLed2" class="button">Toggle</button></p>
			</div>
		</div>
		<div class="content">
			<div class="card2">
				<h2>LDR</h2>
				<p class="state">Leitura atual: <span id="sensor">0</span></p>
				<div id="chart-sensor" class="container"></div>
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
			var chartSensor = new Highcharts.Chart(
			{
				chart:{ renderTo : 'chart-sensor' },
				title: { text: 'Histórico' },
				series: [
				{
					showInLegend: false,
					data: []
				}],
				plotOptions:
				{
					line:
					{
						animation: false,
						dataLabels: { enabled: true }
					},
					series: { color: '#059e8a' }
				},
				xAxis:
				{
				},
				yAxis:
				{
					title: { text: 'Valor analógico (0 - 1023)' }
				},
				credits: { enabled: false }
			});
			chartSensor.setSize(500, 400);
			
			
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
				{
					document.getElementById('icon1').src = data.led1 == "1" ? "http://everaldoprojects.hol.es/LED-ON.png" : "http://everaldoprojects.hol.es/LED-OFF.png";
					document.getElementById('led1').innerHTML = data.led1 == "1" ? "ON" : "OFF";
				}
				if(data.hasOwnProperty('led2'))
				{
					document.getElementById('icon2').src = data.led2 == "1" ? "http://everaldoprojects.hol.es/LED-ON.png" : "http://everaldoprojects.hol.es/LED-OFF.png";
					document.getElementById('led2').innerHTML = data.led2 == "1" ? "ON" : "OFF";
				}
				if(data.hasOwnProperty('time'))
					document.getElementById('time').innerHTML = data.time;
				if(data.hasOwnProperty('sensor'))
				{
					document.getElementById('sensor').innerHTML = data.sensor;
					
					var x = document.getElementById('time').innerHTML;
					var y = parseFloat(data.sensor);
					
					if(chartSensor.series[0].data.length > 40)
						chartSensor.series[0].addPoint([x, y], true, true, true);
					else
						chartSensor.series[0].addPoint([x, y], true, false, true);
				}
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