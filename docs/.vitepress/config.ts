import { defineConfig } from "vitepress";

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
      { text: "Getting Started", link: "/pages/getting-started" },
      { text: "Examples", link: "/pages/examples" },
    ],

    sidebar: [
      {
        text: "Introduction",
        items: [
          { text: "What is chess-library", link: "/pages/what-is" },
          { text: "Getting Started", link: "/pages/getting-started" },
          { text: "PGN parsing", link: "/pages/pgn-parsing" },
          { text: "Examples", link: "/pages/examples" },
        ],
      },
      {
        text: "API",
        items: [
          { text: "Attacks", link: "/pages/attacks" },
          { text: "Bitboard", link: "/pages/bitboard" },
          { text: "Board", link: "/pages/board" },
          { text: "Constants", link: "/pages/constants" },
          { text: "Move", link: "/pages/move" },
          { text: "Move Generation", link: "/pages/move-generation" },
          { text: "Movelist", link: "/pages/movelist" },
          { text: "PGN Utilities", link: "/pages/pgn-utilities" },
          { text: "Piece", link: "/pages/piece" },
          { text: "Piece Type", link: "/pages/piece-type" },
          { text: "File", link: "/pages/file" },
          { text: "Rank", link: "/pages/rank" },
          { text: "Square", link: "/pages/square" },
          { text: "Types", link: "/pages/types" },
        ],
      },
    ],

    socialLinks: [
      { icon: "github", link: "https://github.com/Disservin/chess-library" },
    ],

    outline: "deep",
  },
});
