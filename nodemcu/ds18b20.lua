--------------------------------------------------------------------------------
-- DS18B20 one wire module for NODEMCU
-- NODEMCU TEAM
-- LICENCE: http://opensource.org/licenses/MIT
-- @voborsky, @devsaurus, TerryE  26 Mar 2017
-- modified by @nosoxon for ECE:4880
--------------------------------------------------------------------------------
local modname = ...

local CONVERT_T       = 0x44
local READ_SCRATCHPAD = 0xBE
local READ_POWERSUPPLY= 0xB4
local MODE = 1

local pin, cb, unit = 3
local status
local ssens = "\040\165\210\040\031\025\001\013"
local temp

local debugPrint = function() return end

local function enable_debug()
	debugPrint = function (...) print(tmr.now(),' ', ...) end
end

local function read_scratchpad()
	ow.reset(pin)
	ow.select(pin, ssens)   -- select the  sensor
	ow.write(pin, READ_SCRATCHPAD, MODE)
	local data = ow.read_bytes(pin, 9)

	local t = (data:byte(1)+data:byte(2)*256)
	-- t is actually signed so process the sign bit and adjust for fractional bits
	-- the DS18B20 family has 4 fractional bits and the DS18S20s, 1 fractional bit
	t = ((t <= 32767) and t or t - 65536) * 625
	local crc, b9 = ow.crc8(string.sub(data,1,8)), data:byte(9)

	t = t / 10000
	if math.floor(t) ~= 85 then
		debugPrint(t, crc, b9)
		if crc == b9 then
			temp = t
		end
	end

	if cb then
		node.task.post(node.task.LOW_PRIORITY, function()
			cb(temp) end)
	end
end

local function read_temp(self, lpin, lcb)
	cb, pin = lcb, lpin

	ow.setup(pin)
	ow.reset(pin)
	ow.reset_search(pin)
	if ow.search(pin) == nil then
		node.task.post(node.task.LOW_PRIORITY, function()
			cb(nil) end)
		return
	end

	debugPrint("starting conversion")
	ow.reset(pin)
	ow.select(pin, ssens)  -- select the sensor
	ow.write(pin, CONVERT_T, MODE)  -- and start conversion

	tmr.create():alarm(750, tmr.ALARM_SINGLE, read_scratchpad)
end

 -- Set module name as parameter of require and return module table
local M = { read_temp = read_temp, enable_debug = enable_debug }
_G[modname or 'ds18b20'] = M
return M
