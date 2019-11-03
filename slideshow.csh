while ( 1 == 1 )
	foreach i ( `echo tifs/*.tif `)
		echo $i
		 cp $i ./inframe.tif
		./mcd
		sleep 3
	end
end
