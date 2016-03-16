function b = _test_pf(c)
	define_constants;

	if c.success == 0
		b = 0;
	else
		b = 1;

		%test voltage magnitudes
		for i = 1:size(c.bus)(1)
			b = ((c.bus(i,VM) >= c.bus(i,VMIN)) & (c.bus(i,VM) <= c.bus(i,VMAX)));
			if b == 0
				break;
			end
		end

		if b == 1
			for i = 1:size(c.branch)(1)
				b = (_get_S(c.branch(i,PF), c.branch(i,QF)) <= c.branch(i,RATE_A));
				if b == 0
					break;
				end
			end
		end
	end
end
