static bool NETWORK_BUSY = 0;

void routine_RAMCheck(void)
{
    EspClass esp;
    Serial.printf("%d free RAM\n", esp.getFreeHeap());
}

void routine_LupangHinirang(void)
{
    stream_begin();
    stream_add("Bayang Magiliw,\n");
    stream_add("Perlas ng Silanganan\n");
    stream_add("Alab ng puso\n");
    stream_add("Sa Dibdib mo'y buhay.\n");
    stream_add("\n");
    stream_add("Lupang Hinirang,\n");
    stream_add("Duyan ka ng magiting,\n");
    stream_add("Sa manlulupig\n");
    stream_add("Di ka pasisiil\n");
    stream_add("\n");
    stream_add("Sa dagat at bundok,\n");
    stream_add("Sa simoy at sa langit mong bughaw,\n");
    stream_add("May dilag ang tula\n");
    stream_add("At awit sa paglayang minamahal.\n");
    stream_add("\n");
    stream_add("Ang kislap ng watawat mo'y\n");
    stream_add("Tagumpay na nagniningning;\n");
    stream_add("Ang bituin at araw niya,\n");
    stream_add("Kailan pa ma'y di magdidilim\n");
    stream_add("\n");
    stream_add("Lupa ng araw, ng luwalhati't pagsinta,\n");
    stream_add("Buhay ay langit sa piling mo;\n");
    stream_add("Aming ligaya na pag may mang-aapi,\n");
    stream_add("Ang mamatay nang dahil sa iyo.\n");
    stream_end();
}

void routine_HandlerEmulate(void)
{
    char response[256];
    memset(response, '\0', sizeof(response));

    digitalWrite(LED_BLUE, LOW);
    handler_query_sequence("CDA", response);
    Serial.println(response);
    digitalWrite(LED_BLUE, HIGH);
}

void routine_deploy(void)
{
  if (NETWORK_BUSY == 1) {
    return;
  } else {
    NETWORK_BUSY = 1;
    stream_begin();
    stream_query_all();
    stream_end();
    NETWORK_BUSY = 0;
  }
}

void routine_echo(void)
{
  stream_begin();
  stream_add("Hello world");
  stream_end();
}

// Favorite queries
// DM0 Read number of sorted ICs total
// DM8 Read number of jams total
// DT7 Read "operator assist time"
// CK  Handler status request
// CKS Handler status request
// DD  Read error message line
// CB  Actual temperature value for a control loop

// MUBA
// Mean Units Between Assists
// MUBA = DM0 / DM8

// vim:fdm=syntax:sw=2
