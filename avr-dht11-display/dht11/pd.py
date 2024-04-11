from common.srdhelper import bitpack
import sigrokdecode as srd

_TIME_TOL  = 15
_TIME_WAKE = 18000.0
_TIME_HELO = 80.0
_TIME_BLO  = 50.0
_TIME_HILO = 27.0
_TIME_HIHI = 70.0



class Decoder(srd.Decoder):
	api_version = 3
	id = 'dht11'
	name = 'DHT11'
	longname = 'DHT11'
	desc = 'DHT11 Temperature Sensor Module'
	license = 'gplv3+'
	inputs = ['logic']
	outputs = []

	tags = ['Sensor']

	channels = ({'id': 'io', 'name': 'I/O', 'desc': 'bidirectional data line'},)
	optional_channels = ()

	options = ()

	annotations = (
		('bit',         'Bit'),
		('wake',        'Sensor wakeup signal'),
		('start',       'TX start sequence'),
		('humid-hi',    'Humdity high byte'),
		('humid-lo',    'Humidity low byte'),
		('temp-hi',     'Temperature high byte'),
		('temp-lo',     'Temperature low byte'),
		('parity',      'Parity byte'),
		('stop',        'TX stop sequence'),
		('humdity',     'Humidity'),
		('temperature', 'Temperature'),
		('checksum',    'Checksum')
	)
	annotation_rows = (
		('bits', 'Bits', (0,1,2,8)),
		('fields', 'Fields', (3,4,5,6,7)),
		('values', 'Values', (9,10,11))
	)

	def __init__(self):
		self.reset()

	def metadata(self, key, value):
		if key == srd.SRD_CONF_SAMPLERATE:
			self.samplerate = value

	def start(self):
		self.oid = self.register(srd.OUTPUT_ANN)

	def reset(self):
		self.state = 'SLEEP'
		self.ss_bit = 0
		self.bytes = []
		self.bsum = 0


	def compare_with_tolerance(self, measured, base):
		return (measured >= base * (1 - self.tolerance) and measured <= base * (1 + self.tolerance))

	def calc_rate(self):
		self.tolerance = _TIME_TOL / 100
		self.rwake = int(self.samplerate * _TIME_WAKE / 1000000) - 1
		self.rhelo = int(self.samplerate * _TIME_HELO / 1000000) - 1
		self.rblo  = int(self.samplerate * _TIME_BLO  / 1000000) - 1
		self.rhilo = int(self.samplerate * _TIME_HILO / 1000000) - 1
		self.rhihi = int(self.samplerate * _TIME_HIHI / 1000000) - 1
		print('{} {} {} {} {}'.format(self.rwake, self.rhelo, self.rblo, self.rhilo, self.rhihi))

	def putb(self, data):
		self.put(self.ss_bit, self.samplenum, self.oid, data)

	def putd(self, data):
		self.put(self.ss_start, self.samplenum, self.oid, data)

	def handle_bit(self, tick):
		ret = None
		if self.compare_with_tolerance(tick, self.rblo + self.rhilo):
			ret = 0
		elif self.compare_with_tolerance(tick, self.rblo + self.rhihi):
			ret = 1
		if ret in (0, 1):
			self.putb([0, ['{:d}'.format(ret)]])
			self.data.append(ret)


	def handle_field(self, fid, label, width, next_state):
		self.handle_bit(width)
		if len(self.data) == 8:
			val = bitpack(reversed(self.data))
			self.putd([fid, [f'{label}: 0x{val:02X}']])
			self.data = []
			self.state = next_state

			self.bytes.append([ self.ss_start, val ])
			if fid == 4:
				hval = self.bytes[0][1] + self.bytes[1][1] / 256
				self.put(self.bytes[0][0], self.samplenum, self.oid, [9, [f'Humidity: {hval:.02f}% RH']])
				self.bsum += sum([ v[1] for v in self.bytes ])
				self.bytes = []
			elif fid == 6:
				tval = self.bytes[0][1] + self.bytes[1][1] / 256
				fval = 9 * tval / 5 + 32
				self.bsum += sum([ v[1] for v in self.bytes ])
				self.put(self.bytes[0][0], self.samplenum, self.oid, [10, [f'Temperature: {tval:.02f} °C / {fval:.02f} °F']])
			elif fid == 7:
				valid = val == (self.bsum & 0xff)
				self.put(self.ss_start, self.samplenum, self.oid, [11, ['Valid' if valid else 'Invalid']])

			self.ss_start = self.samplenum

	def decode(self):
		if not self.samplerate:
			raise Exception('samplerate required')
		self.calc_rate()

		while True:
			(dta,) = self.wait({0: 'e'})

			if dta == 1 and self.state != 'STOP':
				continue

			width = self.samplenum - self.ss_bit
			if self.state == 'SLEEP':
				if self.compare_with_tolerance(width, self.rwake):
					self.put(self.ss_bit, self.samplenum, self.oid, [1, ['WAKE']])
					self.state = 'START'
			elif self.state == 'START':
				if self.compare_with_tolerance(width, 2*self.rhelo):
					self.put(self.ss_bit, self.samplenum, self.oid, [2, ['START']])
					self.state = 'HUMID_HI'
					self.data = []
					self.ss_start = self.samplenum
			elif self.state == 'HUMID_HI':
				self.handle_field(3, 'Humidity (HI)', width, 'HUMID_LO')
			elif self.state == 'HUMID_LO':
				self.handle_field(4, 'Humidity (LO)', width, 'TEMP_HI')
			elif self.state == 'TEMP_HI':
				self.handle_field(5, 'Temperature (HI)', width, 'TEMP_LO')
			elif self.state == 'TEMP_LO':
				self.handle_field(6, 'Temperature (LO)', width, 'PARITY')
			elif self.state == 'PARITY':
				self.handle_field(7, 'Parity', width, 'STOP')
			elif self.state == 'STOP':
				if self.compare_with_tolerance(width, self.rblo):
					self.put(self.ss_bit, self.samplenum, self.oid, [8, ['STOP']])
					self.state = 'SLEEP'

			self.ss_bit = self.samplenum
