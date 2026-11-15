// Menu lateral no celular
const toggle = document.getElementById("menuToggle");
const sidebar = document.getElementById("sidebar");
const overlay = document.getElementById("overlay");

function setMenu(open) {
  sidebar.classList.toggle("is-open", open);
  overlay.classList.toggle("is-open", open);
  toggle.setAttribute("aria-expanded", open);
  toggle.setAttribute("aria-label", open ? "Fechar menu" : "Abrir menu");
}

toggle.addEventListener("click", () => setMenu(!sidebar.classList.contains("is-open")));
overlay.addEventListener("click", () => setMenu(false));
document.addEventListener("keydown", (e) => { if (e.key === "Escape") setMenu(false); });
sidebar.querySelectorAll("a").forEach((a) => a.addEventListener("click", () => setMenu(false)));
