#ifndef INDEXHTML_H
#define INDEXHTML_H

const char index_html[] = R"rawliteral(
<!doctype html>
<html lang="uk">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width,initial-scale=1">
  <title>SCADA Lab3 — Pump Station</title>
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
  <h2>SCADA Lab3 — Pump Station</h2>
  <div class="row card">
    <div>
      <div><strong>Measurements</strong></div>
      <div id="meas">
        <div>Water level: <span id="water_level">--</span> %</div>
        <div>Pump: <span id="pump_state">--</span></div>
        <div>Auto pump: <span id="auto_state">--</span></div>
        <div>Alarm code: <span id="alarm_indc">--</span></div>
      </div>
    </div>
    <div style="margin-left:20px">
      <div><strong>LEDs / Status</strong></div>
      <div class="row"><span id="led-on" class="led off"></span> ON</div>
      <div class="row"><span id="led-off" class="led off"></span> OFF</div>
      <div class="row"><span id="led-alarm" class="led off"></span> ALARM</div>
      <div class="row" style="margin-top:8px"><label>Pump</label><button id="btn-toggle">Toggle ON/OFF</button></div>
      <div class="row" style="margin-top:8px"><label>Auto</label><button id="btn-auto">Toggle AUTO</button></div>
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
    document.getElementById('water_level').textContent = j.water_level.toFixed(1);
    document.getElementById('pump_state').textContent = j.pump ? 'ON' : 'OFF';
    document.getElementById('auto_state').textContent = j.auto_pump ? 'ON' : 'OFF';
    document.getElementById('alarm_indc').textContent = j.alarm_indc;

    // LEDs
    setLed('led-on', j.onLed);
    setLed('led-off', j.offLed);
    setLed('led-alarm', j.alarmLed);

  document.getElementById('raw').textContent = JSON.stringify(j, null, 2);

  }catch(e){ console.log('err',e); }
}

function setLed(id, state){
  const el = document.getElementById(id);
  if(state){
    if(id==='led-alarm') el.className='led alarm'; else el.className='led on';
  } else { el.className='led off'; }
}

document.getElementById('btn-toggle').addEventListener('click', async ()=>{
  await fetch('/toggle');
  setTimeout(fetchStatus,200);
});

document.getElementById('btn-auto').addEventListener('click', async ()=>{
  await fetch('/toggle_auto');
  setTimeout(fetchStatus,200);
});

// Real-time updates via Server-Sent Events
if (typeof EventSource !== 'undefined') {
  const es = new EventSource('/events');
  es.onmessage = function(e) {
    try{
      const j = JSON.parse(e.data);
      document.getElementById('water_level').textContent = j.water_level.toFixed(1);
      document.getElementById('pump_state').textContent = j.pump ? 'ON' : 'OFF';
      document.getElementById('auto_state').textContent = j.auto_pump ? 'ON' : 'OFF';
      document.getElementById('alarm_indc').textContent = j.alarm_indc;
      setLed('led-on', j.onLed);
      setLed('led-off', j.offLed);
      setLed('led-alarm', j.alarmLed);
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
