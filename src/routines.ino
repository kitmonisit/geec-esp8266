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

void routine_HandlerQuery(void)
{
    stream_begin();
    stream_add_json("Where");
    stream_add_json("are");
    stream_add_json("you");
    stream_add_json("going");
    stream_add_json("today");
    stream_end();
}

// vim:fdm=syntax

