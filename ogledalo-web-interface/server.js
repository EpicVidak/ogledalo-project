import express from 'express';
import path from 'path';
import SerialPort from 'serialport';
import { fileURLToPath } from 'url';
const app = express();
const port = 3003;
const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);
const Serial_Port = SerialPort.SerialPort;
// Middleware to serve static files
app.use(express.static(path.join(__dirname, 'public')));
app.use(express.json());
app.use(express.urlencoded({ extended: true }));

// Function to detect the Arduino port
const detectArduinoPort = async () => {
  try {
    const ports = await Serial_Port.list();
    const arduinoPort = ports.find(port => port.manufacturer && port.manufacturer.includes('Arduino') || port.friendlyName && port.friendlyName.includes('CH340'));
    if (arduinoPort) {
      return arduinoPort.path;
    } else {
      throw new Error('Arduino not found');
    }
  } catch (error) {
    console.error('Error detecting Arduino port:', error.message);
    process.exit(1);
  }
};
const portPath = await detectArduinoPort();

const serial_port = new Serial_Port({
  path: portPath,
  baudRate: 115200
});

serial_port.on('error', (err) => {
  console.log('Error:', err.message);
});

serial_port.on('open', () => {
  console.log('Serial Port Opened at', portPath);
});

// on serial port data print to console
let ard_data = []
let ard_id = null
function printArduinoData (data) {
  ard_data.push(data)
  if (ard_id) {
    clearTimeout(ard_id)
  }
  ard_id = setTimeout(() => {
    console.log('Arduino data:', ard_data.join(''));
    ard_data = []
  }, 500);
}
serial_port.on('data', (data) => {
  printArduinoData(data.toString());
});


// Serve the HTML file
app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, 'public', 'index.html'));
});

// Handle form submission
app.post('/set-color', async (req, res) => {
  const { red, green, blue, delay, mode } = req.body;

  const dataString = `${red},${green},${blue},${delay},${mode}\n`;
  console.log('Sending color to arduino:', dataString);
  console.log(`btw the server is on http://localhost:${port}`);
  serial_port.write(dataString)
  serial_port.drain((err) => {
    if (err) {
      return console.log('Error on drain:', err.message);
    }
  });
  res.send('Color set successfully');
});

app.listen(port, () => {
  console.log(`Server is running at http://localhost:${port}`);
});
