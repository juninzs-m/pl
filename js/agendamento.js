// agendamento.js - preenche o formulário com os dados do tutor logado e do animal
// cadastrado, e salva cada agendamento no localStorage ("agendamentos").
(() => {
  const user = window.Auth && window.Auth.usuario();
  if (!user) return;   // o auth.js já redirecionou para o login
  const { lerLista, salvarLista } = window.Auth;
  const $ = (id) => document.getElementById(id);
  const form = $("formAgendamento");

  // ---- dados do tutor (vêm do cadastro)
  $("nome_tutor").value = user.nome || "";
  $("telefone").value = user.telefone || "";
  $("email").value = user.email || "";
  $("endereco").value = user.endereco || "";

  // ---- animais cadastrados por este tutor
  const animais = lerLista("animais").filter((a) => a.dono === user.login);
  const seletor = $("animal_salvo");

  function preencherAnimal(i) {
    const a = animais[i] || {};
    $("nome_animal").value = a.nome || "";
    $("especie").value = a.especie || "";
    $("raca").value = a.raca || "";
    $("sexo").value = ["Macho", "Fêmea"].includes(a.sexo) ? a.sexo : "";
    if (!a.nome) $("nascimento").value = "";
  }

  if (animais.length) {
    $("grupoAnimalSalvo").hidden = false;
    seletor.replaceChildren(...animais.map((a, i) => new Option(`${a.nome} (${a.raca})`, i)),
                            new Option("Outro animal (preencher manualmente)", "outro"));
    seletor.addEventListener("change", () => preencherAnimal(seletor.value === "outro" ? -1 : Number(seletor.value)));
    preencherAnimal(0);
  }

  // ---- lista de agendamentos do tutor
  const formatarData = (d) => d ? d.split("-").reverse().join("/") : "";
  function listar() {
    const meus = lerLista("agendamentos").filter((a) => a.dono === user.login);
    $("meusAgendamentos").hidden = meus.length === 0;
    $("listaAgendamentos").replaceChildren(...meus.slice().reverse().map((a) => {
      const li = document.createElement("li");
      li.textContent = `${formatarData(a.data)} às ${a.horario} - ${a.servico} - ${a.nome_animal}`;
      return li;
    }));
  }
  listar();

  // ---- salvar o agendamento
  form.addEventListener("submit", (e) => {
    e.preventDefault();
    const dados = Object.fromEntries(new FormData(form).entries());
    delete dados.animal_salvo;
    const todos = lerLista("agendamentos");
    todos.push({ ...dados, dono: user.login, criadoEm: new Date().toISOString() });
    salvarLista("agendamentos", todos);

    // limpa só os campos do atendimento; tutor e animal continuam preenchidos
    ["servico", "veterinario", "data", "horario", "observacoes"].forEach((id) => { $(id).value = ""; });
    listar();
    alert("Agendamento confirmado! Ele aparece em \"Meus agendamentos\", logo abaixo do formulário.");
  });
})();
