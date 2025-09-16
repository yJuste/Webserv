function getCookie(name) {
  const cookies = document.cookie.split(";").map(c => c.trim());
  for (const c of cookies) {
    if (c.startsWith(name + "=")) {
      return c.split("=")[1];
    }
  }
  return null;
}

const theme = getCookie("theme");
if (theme === "dark") {
  document.body.classList.add("dark");
} else {
  document.body.classList.add("light");
}

document.getElementById("darkBtn").addEventListener("click", () => {
  document.cookie = "theme=dark; Path=/";
  location.reload();
});

document.getElementById("lightBtn").addEventListener("click", () => {
  document.cookie = "theme=light; Path=/";
  location.reload();
});
