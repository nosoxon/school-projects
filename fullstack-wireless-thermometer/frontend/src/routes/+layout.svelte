<script lang=ts>
import Header from '$lib/Header.svelte';
import '../app.css';
</script>


<Header />
<main><slot /></main>
<footer>
	<p><b>Ninecent 2022</b> &#8212; ECE:4880, Lab 1</p>
</footer>


<style>
main {
	display: flex;
	flex-direction: column;
	padding: 1rem;
	width: 100%;
	margin: 0 auto;
	box-sizing: border-box;
}

footer {
	display: flex;
	flex-direction: column;
	justify-content: center;
	align-items: center;
	padding: 40px;
	font-size: 0.8rem;
}
</style>
