<script lang=ts>
import { onMount } from 'svelte'

import ChartJS from 'chart.js/auto'
import 'chartjs-adapter-luxon'

let chart: ChartJS
let chartNode: HTMLCanvasElement

export let unit = 'C'
const unitconv = {
	C(v: number) { return v },
	F(v: number) { return 32 + 9 * v / 5 },
	K(v: number) { return v + 273.15 },
	R(v: number) { return 491.67 + 9 * v / 5 },
}
const unitname = { C: 'Celsius', F: 'Fahrenheit', K: 'Kelvin', R: 'Rankine' }

function ytickcb(value: any, index: any, ticks: any) {
	return unitconv[unit](value).toFixed(2)
}

const data = { datasets: [{
	label: 'Temperature',
	backgroundColor: 'rgb(255, 99, 132)',
	borderColor: 'rgb(255, 99, 132)',
	cubicInterpolationMode: 'monotone',
	data: [],
}] }

const chartAreaBorder = {
	id: 'chartAreaBorder',
	beforeDraw(chart: any, args: any, options: any) {
		const {
			ctx,
			chartArea: { left, top, width, height },
		} = chart
		ctx.save()
		ctx.strokeStyle = options.borderColor
		ctx.lineWidth = options.borderWidth
		ctx.setLineDash(options.borderDash || [])
		ctx.lineDashOffset = options.borderDashOffset
		ctx.strokeRect(left, top, width, height)
		ctx.restore()
	}
};

const options: any = {
	responsive: true,
	scales: {
		x: {
			type: 'realtime',
			realtime: {
				delay: 1 * 1000,
				duration: 20 * 1000,
				ttl: 300 * 1000,
			},
			time: {
				displayFormats: {
					hour: "HH:mm:ss",
					minute: "HH:mm:ss",
					second: "HH:mm:ss",
				},
				unit: "second",
			},
		},
		y: {
			ticks: {
				callback: ytickcb,
			},
			title: {
				display: true,
				text: () => unitname[unit],
				font: {
					size: 18,
					weight: 'bold',
				},
			},
		},
	},
	plugins: {
		chartAreaBorder: {
			borderColor: 'black',
			borderWidth: 0.6,
		},
		legend: { display: false },
		zoom: {
			pan: {
				enabled: true,
				mode: 'x',
			},
			zoom: {
				pinch: { enabled: false, },
				wheel: { enabled: true, },
				mode: 'x',
			},
			limits: {
				x: {
					minDelay: 1 * 1000,
					maxDelay: 300 * 1000,
					minDuration: 1 * 1000,
					maxDuration: 300 * 1000,
				}
			}
		}
	}
}

const config: any = {
	type: 'line',
	data, options,
	plugins: [chartAreaBorder],
}

onMount(async () => {
	const ChartPanning = (await import('chartjs-plugin-zoom')).default
	const ChartStreaming = (await import('chartjs-plugin-streaming')).default

	ChartJS.register(ChartPanning)
	ChartJS.register(ChartStreaming)

	chart = new ChartJS(chartNode, config)
})

export function append(temperature: number) {
	chart.data.datasets[0].data.push({
		x: new Date(),
		y: temperature
	})
}
</script>

<canvas bind:this={chartNode} />
