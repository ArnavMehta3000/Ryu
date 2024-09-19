function PrintHeaders(header)
	print("\n\n----- " .. header .. " -----")
end

function main(argv)
	PrintHeaders("Core")
	os.exec("xmake show -t RyuCore")
	PrintHeaders("Engine")
	os.exec("xmake show -t RyuEngine")
	PrintHeaders("Input")
	os.exec("xmake show -t RyuInput")
	PrintHeaders("Graphics")
	os.exec("xmake show -t RyuGraphics")
	PrintHeaders("Editor")
	os.exec("xmake show -t RyuEditor")
	PrintHeaders("Testbench")
	os.exec("xmake show -t RyuTestbench")
end
