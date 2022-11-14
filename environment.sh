#! /bin/bash
usage() 
{
    cat <<EOF
$(basename ${0}) is a tool for ...

Usage:
    $(basename ${0}) [command] [<options>]

Options:
    --version, -v     print $(basename ${0}) version
    --help, -h        print this
EOF
}


case  ${1} in 
	add)
		ip netns add host1
		ip netns add host2
		ip netns add router
		ip link add name veth1-h1 type veth peer name veth2-rt
		ip link add name veth1-h2 type veth peer name veth3-rt
		ip link set veth1-h1 netns host1
		ip link set veth1-h2 netns host2
		ip link set veth2-rt netns router
		ip link set veth3-rt netns router
		ip netns exec host1 ip addr add 192.168.100.10/24 dev veth1-h1
		ip netns exec host2 ip addr add 192.168.200.10/24 dev veth1-h2
		ip netns exec router ip addr add 192.168.100.20/24 dev veth2-rt
		ip netns exec router ip addr add 192.168.200.20/24 dev veth3-rt
		ip netns exec host1 ip link set veth1-h1 up
		ip netns exec host1 ip link set lo up
		ip netns exec host2 ip link set veth1-h2 up
		ip netns exec host2 ip link set lo up
		ip netns exec router ip link set veth2-rt up
		ip netns exec router ip link set veth3-rt up
		ip netns exec router ip link set lo up
		ip netns exec host1 ip route add 192.168.200.0/24 via 192.168.100.20 dev veth1-h1
		ip netns exec host2 ip route add 192.168.100.0/24 via 192.168.200.20 dev veth1-h2
		ip netns exec router sysctl -w net.ipv4.ip_forward=1
		ip netns exec host1 ip a
		ip netns exec host2 ip a
		ip netns exec router ip a
		ip netns list 
		;;
	del)
		ip netns del host1
		ip netns del host2
		ip netns del router
		ip netns list 
		;;
	show)
		ip netns list
		;;
	help|--help|-h)
		usage
		;;
esac
echo "End of script..."
