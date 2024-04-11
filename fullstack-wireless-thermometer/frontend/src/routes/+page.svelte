<script lang=ts>
import Chart from '$lib/Chart.svelte'
import { onMount } from 'svelte';

let unit = 'C'
let display = 'off'
const unitconv = {
	C(v: number) { return v },
	F(v: number) { return 32 + 9 * v / 5 },
	K(v: number) { return v + 273.15 },
	R(v: number) { return 491.67 + 9 * v / 5 },
}

let temperature = 27.0
let temperatureFormat: string
let chart: Chart

$: temperatureFormat = `${unitconv[unit](temperature).toFixed(2)} °${unit}`

onMount(() => {
	setInterval(async () => {
		let res = await fetch('http://localhost:8080/temperature')
		let body = await res.json()
		temperature = Number(body.temperature)
		chart.append(temperature)
	}, 1000);
})

async function onDisplayChange(event: any) {
	let res = await fetch('http://localhost:8080/display', {
		method: 'POST',
		body: JSON.stringify({ display }),
		headers: { "content-type": "application/json" },
	})
}

</script>

<svelte:head>
	<title>Home</title>
	<meta name=description content="Ninecent Thermal Monitor" />
</svelte:head>

<section>
	<card class=chart-container>
		<Chart bind:this={chart} unit={unit} />
	</card>
	<cards>
		<card>
			<heading>Temperature</heading>
			<temperature>{temperatureFormat}</temperature>

			<heading>Unit</heading>
			<div class=radiogroup>
				<input type=radio name=unit bind:group={unit} value=C id=unit-c checked>
				<input type=radio name=unit bind:group={unit} value=F id=unit-f>
				<input type=radio name=unit bind:group={unit} value=K id=unit-k>
				<input type=radio name=unit bind:group={unit} value=R id=unit-r>

				<label for=unit-c class=unit-c title=Celsius>°C</label>
				<label for=unit-f class=unit-f title=Fahrenheit>°F</label>
				<label for=unit-k class=unit-k title=Kelvin>K</label>
				<label for=unit-r class=unit-r title=Rankine>°R</label>
			</div>

			<heading>Handset Display</heading>
			<div class=radiogroup>
				<input type=radio name=display bind:group={display} on:change={onDisplayChange} value=off id=display-off checked>
				<input type=radio name=display bind:group={display} on:change={onDisplayChange} value=on  id=display-on>

				<label for=display-off class=display-off title=Off>Off</label>
				<label for=display-on  class=display-on  title=On>On</label>
			</div>
		</card>
		<card>
			<heading>SMS Updates</heading>
		</card>
	</cards>
</section>

<style>
section {
	display: flex;
	flex-flow: row wrap;
	justify-content: space-between;
}

.chart-container {
	flex: 3;
}

cards {
	margin-left: 1rem;
	flex: 1;
}

card {
	display: block;
	background: white;
	box-shadow: 0 0 3px 1px rgba(0, 0, 0, 0.1);
	border-radius: 12px;
	padding: 1rem;
	font-size: 1.4rem;
	margin-bottom: 1rem;
}

card > heading {
	font-size: 0.8rem;
	font-weight: bold;
	display: block;
	color: #666666;
	margin-top: 0.8rem;
	margin-bottom: 0.4rem;
}

card > heading:first-child {
	margin-top: 0;
}

temperature {
	font-size: 4rem;
}

.radiogroup input[type=radio] {
	display: none;
}

.radiogroup {
	display: flex;
}

.radiogroup > label {
	flex: 1;
	background: #ddd;
	padding: 0.2rem;
	border: 2px solid transparent;
	cursor: pointer;
	transition: all 0.25s ease;

	text-align: center;

	font-size: 1rem;
}

.radiogroup > label:first-of-type {
	border-radius: 5px 0 0 5px;
}

.radiogroup > label:last-of-type {
	border-radius: 0 5px 5px 0;
}

.radiogroup > label:hover {
	background: #d5bbf7;
}

#unit-c:checked ~ label.unit-c,
#unit-f:checked ~ label.unit-f,
#unit-k:checked ~ label.unit-k,
#unit-r:checked ~ label.unit-r,
#display-off:checked ~ label.display-off,
#display-on:checked ~ label.display-on {
	border-color: #8E49E8;
	background: #d5bbf7;
	font-weight: bold;
}

</style>
