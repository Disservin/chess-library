import { defineConfig } from "vitepress";
import { getSidebar } from "vitepress-plugin-auto-sidebar";

// https://vitepress.dev/reference/site-config
export default defineConfig({
	title: "C++ Chess Library",
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
			{ text: "Examples", link: "/pages/examples" },
		],

		sidebar: getSidebar({
			contentRoot: "/docs",
			contentDirs: ["pages"],
			collapsible: false,
			collapsed: false,
		}),

		socialLinks: [
			{ icon: "github", link: "https://github.com/Disservin/chess-library" },
		],

		outline: "deep",
	},
});
