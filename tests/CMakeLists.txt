
# Testes de cenários baseados nos arquivos em data/
# Cada teste executa o simulador em modo JSON e valida campos com grep.

function(add_json_assert_test NAME DATA_FILE)
	# Cria um comando que executa o simulador UMA única vez e valida múltiplos padrões
	set(options)
	set(oneValueArgs)
	set(multiValueArgs EXPECT)
	cmake_parse_arguments(JAT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	if (NOT JAT_EXPECT)
		message(FATAL_ERROR "add_json_assert_test requer pelo menos um EXPECT=<padrao>")
	endif()

	# Monta cadeia de asserções independentes sobre a mesma saída
	set(ASSERTS "true")
	foreach(PATTERN IN LISTS JAT_EXPECT)
		string(REPLACE "'" "'\\''" ESCAPED "${PATTERN}")
		set(ASSERTS "${ASSERTS} && echo \"$OUT\" | grep -F -q '${ESCAPED}'")
	endforeach()

	add_test(NAME ${NAME}
		COMMAND bash -c "OUT=\`\"${CMAKE_BINARY_DIR}/bin/simulador\" \"${CMAKE_SOURCE_DIR}/data/${DATA_FILE}\" --json-only\`; echo \"$OUT\"; ${ASSERTS}"
	)
endfunction()

# EXPECTATIVAS ALINHADAS AO NOME DO ARQUIVO

# 1) beco_sem_saida: deve escapar sem encontro
add_json_assert_test(beco_sem_saida beco_sem_saida EXPECT
	"\"sobreviveu\": true"
	"\"encontro\": { \"ok\": false"
)

# 2) escape: deve escapar
add_json_assert_test(escape_deve_escapar escape EXPECT
	"\"sobreviveu\": true"
)

# 3) fuga: deve escapar
add_json_assert_test(fuga_deve_escapar fuga EXPECT
	"\"sobreviveu\": true"
)

# 4) morte_fome: deve morrer de fome (sem encontro)
add_json_assert_test(morte_fome_sem_encontro morte_fome EXPECT
	"\"sobreviveu\": false"
	"\"encontro\": { \"ok\": false"
)

# 5) perseguicao_imediata: deve haver encontro
add_json_assert_test(perseguicao_imediata_com_encontro perseguicao_imediata EXPECT
	"\"encontro\": { \"ok\": true"
)

# 6) teste_distante: minotauro longe, espera-se sem encontro
add_json_assert_test(teste_distante_sem_encontro teste_distante EXPECT
	"\"encontro\": { \"ok\": false"
)
