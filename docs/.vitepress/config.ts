import { defineConfig } from "vitepress";

// https://vitepress.dev/reference/site-config
export default defineConfig({
	title: "C++ Chess",
	description: "Documentation for chess-library",
	base: "/chess-library/",
	themeConfig: {
		search: {
			provider: "local",
		},
		editLink: {
			pattern:
				"https://github.com/Disservin/chess-library/edit/master/docs/:path",
		},
		// https://vitepress.dev/reference/default-theme-config
		nav: [
			{ text: "Home", link: "/" },
			{ text: "Usage", link: "/pages/usage" },
			{ text: "Examples", link: "/pages/example" },
		],

		sidebar: [
			{
				text: "Documentation",
				items: [
					{ text: "Usage", link: "/pages/usage" },
					{ text: "Types", link: "/pages/types" },
					{ text: "Constants", link: "/pages/constants" },
					{ text: "Board Object", link: "/pages/board-object" },
					{ text: "Move", link: "/pages/move-object" },
					{ text: "Movelist Object", link: "/pages/movelist-object" },
					{ text: "Intrinsic Functions", link: "/pages/intrinsic" },
					{ text: "Attacks", link: "/pages/attacks" },
					{ text: "Helper Functions", link: "/pages/helpers" },
					{ text: "Move Generation", link: "/pages/move-gen" },
				],
			},
		],

		socialLinks: [
			{ icon: "github", link: "https://github.com/Disservin/chess-library" },
		],
	},
});
