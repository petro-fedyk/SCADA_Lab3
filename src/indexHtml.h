#ifndef INDEXHTML_H
#define INDEXHTML_H

const char index_html[] = R"rawliteral(
<!doctype html>
<html lang="uk">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width,initial-scale=1">
  <title>SCADA Lab3 — Motor Control</title>
  <style>
    body{font-family:Arial,Helvetica,sans-serif;margin:12px;color:#222}
    .row{display:flex;gap:12px;align-items:center;margin:8px 0}
    .card{border:1px solid #ddd;padding:12px;border-radius:6px}
    .led{width:18px;height:18px;border-radius:50%;display:inline-block;margin-right:6px;vertical-align:middle}
    .led.off{background:#333}
    .led.on{background:#0c0}
    .led.alarm{background:#c00}
    label{min-width:110px;display:inline-block}
    input[type=number]{width:110px}
    button{padding:6px 10px}
    pre{background:#f6f6f6;padding:8px;border-radius:6px}
  </style>
</head>
<body>
  <h2>SCADA Lab3 — Motor Control</h2>
  <div class="row card">
    <div>
      <div><strong>Measurements</strong></div>
      <div id="meas">
        <div>Temperature: <span id="temperature">--</span> °C</div>
        <div>Motor power: <span id="motor_power">--</span> %</div>
        <div>Motor running: <span id="motor_running">--</span></div>
        <div>PWM: <span id="motor_pwm">--</span></div>
      </div>
    </div>
    <div style="margin-left:20px">
      <div><strong>LEDs / Status</strong></div>
      <div class="row"><span id="led-in1" class="led off"></span> IN1</div>
      <div class="row"><span id="led-in2" class="led off"></span> IN2</div>
    </div>
  </div>

  <div class="card" style="margin-top:12px">
    <div><strong>Raw status</strong></div>
    <pre id="raw">--</pre>
  </div>

<script>
async function fetchStatus(){
  try{
    const r = await fetch('/status');
    const j = await r.json();
    document.getElementById('temperature').textContent = j.temperature.toFixed(1);
    document.getElementById('motor_power').textContent = j.motor_power;
    document.getElementById('motor_running').textContent = j.motor_running ? 'ON' : 'OFF';
    document.getElementById('motor_pwm').textContent = j.pwm;

  // LEDs
  setLed('led-in1', j.in1);
  setLed('led-in2', j.in2);

  document.getElementById('raw').textContent = JSON.stringify(j, null, 2);

  }catch(e){ console.log('err',e); }
}

function setLed(id, state){
  const el = document.getElementById(id);
  if(state){
    if(id==='led-alarm') el.className='led alarm'; else el.className='led on';
  } else { el.className='led off'; }
}

// Real-time updates via Server-Sent Events
if (typeof EventSource !== 'undefined') {
  const es = new EventSource('/events');
  es.onmessage = function(e) {
    try{
      const j = JSON.parse(e.data);
  document.getElementById('temperature').textContent = j.temperature.toFixed(1);
  document.getElementById('motor_power').textContent = j.motor_power;
  document.getElementById('motor_running').textContent = j.motor_running ? 'ON' : 'OFF';
  document.getElementById('motor_pwm').textContent = j.pwm;
  setLed('led-in1', j.in1);
  setLed('led-in2', j.in2);
      document.getElementById('raw').textContent = JSON.stringify(j, null, 2);
    }catch(err){ console.log(err); }
  };
  es.onerror = function(){ console.log('SSE error'); };
} else {
  // fallback to polling
  fetchStatus();
  setInterval(fetchStatus, 2000);
}
</script>
</body>
</html>
)rawliteral";

#endif
