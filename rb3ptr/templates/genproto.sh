for i in inline nodecmp datacmp; do
        cat rb3ptr-"$i".tpl | sed '/^{/,/^}/d' | sed 's/)$/);/' > rb3ptr-"$i"-proto.tpl
done
