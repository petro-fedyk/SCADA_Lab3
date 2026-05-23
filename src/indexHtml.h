#ifndef INDEXHTML_H
#define INDEXHTML_H

const char index_html[] = R"rawliteral(
<!doctype html>
<html lang="uk">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width,initial-scale=1">
  <title>SCADA Lab3 — Washing Machine</title>
  <style>
    body{font-family:Arial,Helvetica,sans-serif;margin:12px;color:#222}
    .row{display:flex;gap:12px;align-items:center;margin:8px 0}
    .card{border:1px solid #ddd;padding:12px;border-radius:6px}
    .led{width:18px;height:18px;border-radius:50%;display:inline-block;margin-right:6px;vertical-align:middle}
    .led.off{background:#333}
    .led.on{background:#0c0}
    .led.red{background:#c00}
    .led.yellow{background:#e6c200}
    label{min-width:110px;display:inline-block}
    input[type=number]{width:110px}
    button{padding:6px 10px}
    pre{background:#f6f6f6;padding:8px;border-radius:6px}
    input[type=range]{width:220px}
  </style>
</head>
<body>
  <h2>SCADA Lab3 — Washing Machine</h2>
  <div class="row card">
    <div>
      <div><strong>Measurements</strong></div>
      <div id="meas">
        <div>Temperature: <span id="temperature">--</span> °C</div>
        <div>Mode: <span id="mode">--</span></div>
  <div>Motor speed: <span id="motorSpeed">--</span> %</div>
      </div>
      <div class="row" style="margin-top:10px">
  <label>Motor speed</label>
  <input id="motorSlider" type="range" min="0" max="100" value="0" disabled>
      </div>
    </div>
    <div style="margin-left:20px">
      <div><strong>Indicators</strong></div>
  <div class="row"><span id="led-relay" class="led off"></span> Motor enabled</div>
      <div class="row"><span id="led-red" class="led off"></span> Red indicator</div>
      <div class="row"><span id="led-yellow" class="led off"></span> Yellow indicator</div>
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
  document.getElementById('temperature').textContent = j.temperature.toFixed(2);
  document.getElementById('mode').textContent = j.mode;
  document.getElementById('motorSpeed').textContent = j.motorSpeed ?? j.powerPercent;
  document.getElementById('motorSlider').value = j.motorSpeed ?? j.powerPercent;

  setLed('led-relay', j.relayOn);
  setLed('led-red', j.redLed, 'red');
  setLed('led-yellow', j.yellowLed, 'yellow');

  document.getElementById('raw').textContent = JSON.stringify(j, null, 2);

  }catch(e){ console.log('err',e); }
}

function setLed(id, state, kind){
  const el = document.getElementById(id);
  if(state){
    if(kind==='red') el.className='led red';
    else if(kind==='yellow') el.className='led yellow';
    else el.className='led on';
  } else {
    el.className='led off';
  }
}

// Real-time updates via Server-Sent Events
if (typeof EventSource !== 'undefined') {
  const es = new EventSource('/events');
  es.onmessage = function(e) {
    try{
      const j = JSON.parse(e.data);
  document.getElementById('temperature').textContent = j.temperature.toFixed(2);
  document.getElementById('mode').textContent = j.mode;
  document.getElementById('motorSpeed').textContent = j.motorSpeed ?? j.powerPercent;
  document.getElementById('motorSlider').value = j.motorSpeed ?? j.powerPercent;
  setLed('led-relay', j.relayOn);
  setLed('led-red', j.redLed, 'red');
  setLed('led-yellow', j.yellowLed, 'yellow');
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
