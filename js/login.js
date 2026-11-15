// login.js - menu lateral, login, cadastro de usuario e cadastro de animais.
// Este arquivo e autossuficiente: nao precisa do script.js.
// Os dados ficam no localStorage do navegador (nosso "banco de dados").

// ---------- Menu lateral no celular (igual ao script.js) ----------
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

// Ano atual no rodape
const year = document.getElementById("year");
if (year) year.textContent = new Date().getFullYear();

// Cria um elemento: el("p", "classe", "texto")
function el(tag, className, text) {
  const e = document.createElement(tag);
  if (className) e.className = className;
  if (text) e.textContent = text;
  return e;
}

// ---------- Login, cadastro e animais ----------
(() => {
  // Lista de racas. Se o racas.json nao carregar (ex.: abrir o arquivo direto,
  // sem servidor), esta lista continua sendo usada.
  const RACAS_PADRAO = [
    "Sem raça definida (SRD)", "Labrador", "Golden Retriever", "Pastor Alemão",
    "Bulldog Francês", "Bulldog Inglês", "Poodle", "Shih Tzu", "Yorkshire", "Pug",
    "Rottweiler", "Beagle", "Husky Siberiano", "Border Collie", "Dachshund (Salsicha)",
    "Lhasa Apso", "Pinscher", "Maltês", "Chihuahua", "Pit Bull"
  ];

  // ---------- Banco de dados (localStorage) ----------
  const lerLista = (chave) => {
    try { return JSON.parse(localStorage.getItem(chave)) || []; }
    catch { return []; }
  };
  const salvarLista = (chave, lista) => localStorage.setItem(chave, JSON.stringify(lista));
  const loginAtual = () => localStorage.getItem("usuarioLogado") || "";

  // Mostra uma mensagem de erro (ou de sucesso, se ok = true)
  function mostrarMsg(id, texto, ok = false) {
    const msg = document.getElementById(id);
    if (!msg) return;
    msg.textContent = texto;
    msg.classList.toggle("msg--ok", ok);
  }

  // ---------- Pagina de cadastro ----------
  const formCadastro = document.getElementById("formCadastro");
  if (formCadastro) {
    formCadastro.addEventListener("submit", (e) => {
      e.preventDefault();
      const nome = document.getElementById("cadNome").value.trim();
      const telefone = document.getElementById("cadTelefone").value.trim();
      const email = document.getElementById("cadEmail").value.trim();
      const endereco = document.getElementById("cadEndereco").value.trim();
      const login = document.getElementById("cadLogin").value.trim();
      const senha = document.getElementById("cadSenha").value;
      const confirma = document.getElementById("cadConfirma").value;

      if (!nome || !login || !senha || !telefone || !email || !endereco) return mostrarMsg("msgCadastro", "Preencha todos os campos.");
      if (senha.length < 4) return mostrarMsg("msgCadastro", "A senha precisa ter pelo menos 4 caracteres.");
      if (senha !== confirma) return mostrarMsg("msgCadastro", "As senhas não conferem.");

      const usuarios = lerLista("usuarios");
      if (usuarios.some((u) => u.login === login)) {
        return mostrarMsg("msgCadastro", "Esse login já existe. Escolha outro.");
      }

      usuarios.push({ nome, login, senha, telefone, email, endereco });
      salvarLista("usuarios", usuarios);
      // já entra na conta e segue para o cadastro do primeiro animal
      localStorage.setItem("usuarioLogado", login);
      window.location.href = "animais.html?primeiro=1";
    });
  }

  // ---------- Pagina de login ----------
  const formLogin = document.getElementById("formLogin");
  if (formLogin) {
    if (new URLSearchParams(window.location.search).has("novo")) {
      mostrarMsg("msgLogin", "Conta criada! Agora é só entrar.", true);
    }

    formLogin.addEventListener("submit", (e) => {
      e.preventDefault();
      const login = document.getElementById("loginUsuario").value.trim();
      const senha = document.getElementById("loginSenha").value;

      const achou = lerLista("usuarios").find((u) => u.login === login && u.senha === senha);
      if (!achou) return mostrarMsg("msgLogin", "Login ou senha incorretos.");

      localStorage.setItem("usuarioLogado", achou.login);
      window.location.href = "inicio.html";
    });
  }

  // ---------- Pagina de animais ----------
  const areaAnimais = document.getElementById("areaAnimais");
  if (areaAnimais) {
    const usuario = lerLista("usuarios").find((u) => u.login === loginAtual());

    if (!usuario) {
      // ninguem logado: mostra o aviso
      document.getElementById("avisoLogin").hidden = false;
    } else {
      areaAnimais.hidden = false;
      document.getElementById("boasVindas").textContent = `Olá, ${usuario.nome}!`;
      iniciarAnimais(usuario);
    }
  }

  function preencherRacas(lista) {
    const select = document.getElementById("animalRaca");
    select.replaceChildren(...lista.map((raca) => {
      const op = el("option", "", raca);
      op.value = raca;
      return op;
    }));
  }

  function listarAnimais(usuario) {
    const box = document.getElementById("listaAnimais");
    const meus = lerLista("animais").filter((a) => a.dono === usuario.login);

    if (meus.length === 0) {
      box.replaceChildren(el("p", "empty", "Você ainda não tem animais cadastrados."));
      return;
    }

    box.replaceChildren(...meus.map((a) => {
      const card = el("article", "card pet");

      const ico = el("span", "card__icon", a.especie === "Gato" ? "🐱" : "🐶");
      ico.setAttribute("aria-hidden", "true");

      const tags = el("div", "pet__tags");
      tags.append(
        el("span", "tag", a.sexo),
        el("span", "tag", `${a.idade} ${a.idade === "1" ? "ano" : "anos"}`),
        el("span", "tag", `${a.peso} kg`),
        el("span", a.vacina === "Sim" ? "tag" : "tag tag--alert",
           a.vacina === "Sim" ? "Vacinas em dia" : "Vacinas atrasadas")
      );

      card.append(ico, el("h3", "", a.nome), el("p", "pet__race", a.raca), tags);
      if (a.obs) card.append(el("p", "pet__obs", a.obs));
      return card;
    }));
  }

  function iniciarAnimais(usuario) {
    preencherRacas(RACAS_PADRAO);
    fetch("data/racas.json")
      .then((r) => { if (!r.ok) throw new Error(r.status); return r.json(); })
      .then((dados) => preencherRacas(dados.racas))
      .catch(() => console.info("racas.json não carregado; usando a lista do login.js."));

    listarAnimais(usuario);

    // primeiro acesso: mostra o aviso e o botão para seguir ao início
    if (new URLSearchParams(location.search).has("primeiro")) {
      document.getElementById("primeiroAcesso").hidden = false;
      const continuar = document.getElementById("continuar");
      continuar.hidden = lerLista("animais").every((a) => a.dono !== usuario.login);
    }

    const form = document.getElementById("formAnimal");
    form.addEventListener("submit", (e) => {
      e.preventDefault();
      const animal = {
        nome: document.getElementById("animalNome").value.trim(),
        especie: document.getElementById("animalEspecie").value,
        raca: document.getElementById("animalRaca").value,
        idade: document.getElementById("animalIdade").value,
        peso: document.getElementById("animalPeso").value,
        sexo: document.getElementById("animalSexo").value,
        vacina: document.getElementById("animalVacina").value,
        obs: document.getElementById("animalObs").value.trim(),
        dono: usuario.login
      };

      if (!animal.nome) return mostrarMsg("msgAnimal", "Digite o nome do animal.");

      const animais = lerLista("animais");
      animais.push(animal);
      salvarLista("animais", animais);

      form.reset();
      mostrarMsg("msgAnimal", `${animal.nome} foi cadastrado!`, true);
      listarAnimais(usuario);
      const continuar = document.getElementById("continuar");
      if (continuar) continuar.hidden = false;
    });
  }
})();
