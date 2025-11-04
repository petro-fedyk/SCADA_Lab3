#ifndef INDEXHTML_H
#define INDEXHTML_H

const char index_html[] = R"rawliteral(
<!doctype html>
<html lang="uk">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width,initial-scale=1">
  <title>SCADA Lab3 — Control</title>
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
  <h2>SCADA Lab3 — Web Interface</h2>
  <div class="row card">
    <div>
      <div><strong>Measurements</strong></div>
      <div id="meas">
        <div>Temperature: <span id="temperature">--</span> °C</div>
        <div>Voltage: <span id="voltage">--</span> V</div>
        <div>Current: <span id="current">--</span> mA</div>
        <div>Power: <span id="power">--</span> mW</div>
        <div>Battery: <span id="battery">--</span> %</div>
      </div>
    </div>
    <div style="margin-left:20px">
      <div><strong>LEDs / Status</strong></div>
      <div class="row"><span id="led-on" class="led off"></span> ON</div>
      <div class="row"><span id="led-off" class="led off"></span> OFF</div>
      <div class="row"><span id="led-alarm" class="led off"></span> ALARM</div>
      <div class="row" style="margin-top:8px"><label>Station</label><button id="btn-toggle">Toggle ON/OFF</button></div>
    </div>
  </div>

  <div class="card" style="margin-top:12px">
    <div><strong>Alarm thresholds</strong></div>
    <div class="row"><label>Battery % &lt; </label><input id="th-batt" type="number" min="0" max="100"></div>
    <div class="row"><label>Voltage &gt; </label><input id="th-volt" type="number" step="0.01"></div>
    <div class="row"><label>Current &gt; </label><input id="th-curr" type="number" step="0.1"></div>
    <div class="row"><button id="btn-save">Save thresholds</button><span id="save-res" style="margin-left:8px"></span></div>
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
    document.getElementById('temperature').textContent = j.temperature.toFixed(2);
    document.getElementById('voltage').textContent = j.voltage.toFixed(2);
    document.getElementById('current').textContent = j.current.toFixed(2);
    document.getElementById('power').textContent = j.power.toFixed(2);
    document.getElementById('battery').textContent = j.battery;

    // LEDs
    setLed('led-on', j.onLed);
    setLed('led-off', j.offLed);
    setLed('led-alarm', j.alarmLed);

    document.getElementById('raw').textContent = JSON.stringify(j, null, 2);

    // thresholds
    document.getElementById('th-batt').value = j.thresholds.battery;
    document.getElementById('th-volt').value = j.thresholds.voltage;
    document.getElementById('th-curr').value = j.thresholds.current;

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

document.getElementById('btn-save').addEventListener('click', async ()=>{
  const b = document.getElementById('th-batt').value;
  const v = document.getElementById('th-volt').value;
  const c = document.getElementById('th-curr').value;
  const body = new URLSearchParams();
  body.append('battery', b);
  body.append('voltage', v);
  body.append('current', c);
  const res = await fetch('/settings', {method:'POST', body});
  const txt = await res.text();
  document.getElementById('save-res').textContent = txt;
  setTimeout(()=>document.getElementById('save-res').textContent='',2000);
});

// Real-time updates via Server-Sent Events
if (typeof EventSource !== 'undefined') {
  const es = new EventSource('/events');
  es.onmessage = function(e) {
    try{
      const j = JSON.parse(e.data);
      document.getElementById('temperature').textContent = j.temperature.toFixed(2);
      document.getElementById('voltage').textContent = j.voltage.toFixed(2);
      document.getElementById('current').textContent = j.current.toFixed(2);
      document.getElementById('power').textContent = j.power.toFixed(2);
      document.getElementById('battery').textContent = j.battery;
      setLed('led-on', j.onLed);
      setLed('led-off', j.offLed);
      setLed('led-alarm', j.alarmLed);
      document.getElementById('raw').textContent = JSON.stringify(j, null, 2);
      document.getElementById('th-batt').value = j.thresholds.battery;
      document.getElementById('th-volt').value = j.thresholds.voltage;
      document.getElementById('th-curr').value = j.thresholds.current;
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
