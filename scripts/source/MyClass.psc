ScriptName MyClass


String Function HelloWorld() Global
	return "Hello world!"
EndFunction


Function RegisterForMyEvent(Form a_thisForm) Global Native


Event MyEvent(Form a_form, ObjectReference a_ref, Int a_num, Float a_flt, String a_str, Form[] a_arr, Int[] a_numArr)
EndEvent
