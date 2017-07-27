static bool NETWORK_BUSY = 0;

void routine_RAMCheck(void)
{
    EspClass esp;
    // Serial.printf("%d free RAM\n", esp.getFreeHeap());
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
    const uint8_t element_size = 64;
    char query_array[6][element_size];

    NETWORK_BUSY = 1;
    stream_begin();
    misc_editArr(query_array, "CA", "CAS", "CB", "CD", "CDA", sizeof query_array, element_size);stream_query(query_array);
    stream_end();
    stream_begin();
    misc_editArr(query_array, "CDC", "CDI", "CDN", "CDNH", "CDNL", sizeof query_array, element_size);stream_query(query_array);
    stream_end();
    stream_begin();
    misc_editArr(query_array, "CDNU", "CDP", "CDS", "CDZ", "CE", sizeof query_array, element_size);stream_query(query_array);
    stream_end();
    stream_begin();
    misc_editArr(query_array, "CEC", "CER", "CF", "CK", "CKA", sizeof query_array, element_size);stream_query(query_array);
    stream_end();
    stream_begin();
    misc_editArr(query_array, "CKLD", "CKLS", "CKS", "CKTI", "CML", sizeof query_array, element_size);stream_query(query_array);
    stream_end();
    stream_begin();
    misc_editArr(query_array, "CMM", "CN", "COB", "COF", "CQC", sizeof query_array, element_size);stream_query(query_array);
    stream_end();
    stream_begin();
    misc_editArr(query_array, "CQI", "CQL", "CQO", "CQP", "CQU", sizeof query_array, element_size);stream_query(query_array);
    stream_end();
    stream_begin();
    misc_editArr(query_array, "CSO", "CSOT", "CSR", "CSS", "CSSA", sizeof query_array, element_size);stream_query(query_array);
    stream_end();
    stream_begin();
    misc_editArr(query_array, "CSS1T", "CSS2T", "CST", "CSW", "CSWA", sizeof query_array, element_size);stream_query(query_array);
    stream_end();
    stream_begin();
    misc_editArr(query_array, "CVC", "CZASE", "CZD", "CZETP", "CZ 1 port status?", sizeof query_array, element_size);stream_query(query_array);
    stream_end();
    stream_begin();
    misc_editArr(query_array, "CZ 2 port status?", "CZ 3 port status?", "CZSPA", "CZSPC", "CZTCBASE", sizeof query_array, element_size);stream_query(query_array);
    stream_end();
    stream_begin();
    misc_editArr(query_array, "CZTCOR", "CZTPA", "CZTPC", "CZPWD", "DD", sizeof query_array, element_size);stream_query(query_array);
    stream_end();
    stream_begin();
    misc_editArr(query_array, "DF", "DFA", "DM0", "DM1", "DM2", sizeof query_array, element_size);stream_query(query_array);
    stream_end();
    stream_begin();
    misc_editArr(query_array, "DM3", "DM4", "DM5", "DM6", "DM7", sizeof query_array, element_size);stream_query(query_array);
    stream_end();
    stream_begin();
    misc_editArr(query_array, "DM8", "DM9", "DM10", "DM11", "DM12", sizeof query_array, element_size);stream_query(query_array);
    stream_end();
    stream_begin();
    misc_editArr(query_array, "DN0", "DN1", "DN2", "DN3", "DPY", sizeof query_array, element_size);stream_query(query_array);
    stream_end();
    stream_begin();
    misc_editArr(query_array, "DPYD", "DPYP", "DPYR", "DPYS", "DPYT", sizeof query_array, element_size);stream_query(query_array);
    stream_end();
    stream_begin();
    misc_editArr(query_array, "DPYY", "DT0", "DT1", "DT2", "DT3", sizeof query_array, element_size);stream_query(query_array);
    stream_end();
    stream_begin();
    misc_editArr(query_array, "DT4", "DT5", "DT6", "DT7", "DT8", sizeof query_array, element_size);stream_query(query_array);
    stream_end();
    stream_begin();
    misc_editArr(query_array, "DT9", "DT10", "DV 0", "DV 1", "DV 2", sizeof query_array, element_size);stream_query(query_array);
    stream_end();
    stream_begin();
    misc_editArr(query_array, "DV 3", "DV 4", "DV 5", "DV 6", "DV 7", sizeof query_array, element_size);stream_query(query_array);
    stream_end();
    stream_begin();
    misc_editArr(query_array, "DV 8", "DV 9", "DV 10", "DV 11", "DV 12", sizeof query_array, element_size);stream_query(query_array);
    stream_end();
    stream_begin();
    misc_editArr(query_array, "DV 13", "DV 14", "DV 15", "DV 16", "DV 17", sizeof query_array, element_size);stream_query(query_array);
    stream_end();
    stream_begin();
    misc_editArr(query_array, "DV 18", "DV 19", "DV 20", "DV 21", "DV 22", sizeof query_array, element_size);stream_query(query_array);
    stream_end();
    stream_begin();
    misc_editArr(query_array, "DV 23", "DV 24", "DV 25", "DV 26", "DV 27", sizeof query_array, element_size);stream_query(query_array);
    stream_end();
    stream_begin();
    misc_editArr(query_array, "DV 28", "DV 29", "DV 30", "DV 31", "DY", sizeof query_array, element_size);stream_query(query_array);
    stream_end();
    stream_begin();
    misc_editArr(query_array, "FAR", "HVRC", "HVRL", "HVRO", "HVRU", sizeof query_array, element_size);stream_query(query_array);
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

void misc_editArr(
          char arr[6][64],
    const char *q0,
    const char *q1,
    const char *q2,
    const char *q3,
    const char *q4,
    uint16_t arr_size,
    uint8_t  element_size)
{
  uint8_t idx = 0;
  uint8_t num_elements = arr_size/element_size;
  memset(arr, 0, arr_size);

  sprintf(arr[0], "%s", q0);
  sprintf(arr[1], "%s", q1);
  sprintf(arr[2], "%s", q2);
  sprintf(arr[3], "%s", q3);
  sprintf(arr[4], "%s", q4);
  memset(arr[5], '\0', element_size);
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

// vim:fdm=syntax:nowrap:sw=2
