ds18b20 = require("ds18b20")

local temp = 27.0
local tpin = 1 -- D1
local dpin = 3 -- D3

uri = "http://10.0.0.185:8080/temperature"
hdrs = "Content-Type: application/x-www-form-urlencoded\r\n"

local err_digits = {12, 13, 13, 11}
local temp_digits = {0, 0, 0, 0}

function send_spi_temp(t)
	t = math.floor(math.abs(t) * 10)
	temp_digits[1] = (t < 0) and 0xa or math.floor(t / 1000)
	t = t % 1000
	temp_digits[2] = math.floor(t / 100)
	temp_digits[3] = math.floor((t % 100) / 10)
	temp_digits[4] = math.floor(t % 10)
	spi.send(1, 0x66, temp_digits, 0x77)
end

function send_spi_err()
	spi.send(1, 0x66, err_digits, 0x77)
end

function post_cb(status, body, headers)
	if status == 200 then -- display OFF
		gpio.write(dpin, gpio.HIGH)
	elseif status == 201 then -- display ON
		gpio.write(dpin, gpio.LOW)
	end
end

function read_temperature()
	ds18b20:read_temp(tpin, set_temp)
end

ttmr = tmr.create()
function set_temp(t)
	local body = "t=error"
	if t then
		body = string.format("t=%.3f", t)
		temp = t
		print(temp)

		send_spi_temp(temp)
	else
		print("disconnected")
		send_spi_err()
	end
	http.post(uri, hdrs, body, post_cb)
	ttmr:alarm(1000, tmr.ALARM_SINGLE, read_temperature)
end

spi.setup(1, spi.MASTER, spi.CPOL_HIGH, spi.CPHA_LOW, spi.DATABITS_8, 0, spi.HALFDUPLEX)

gpio.mode(dpin, gpio.OUTPUT)
gpio.write(dpin, gpio.HIGH)

read_temperature()
