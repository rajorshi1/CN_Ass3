"""Custom topology example

Two directly connected switches plus a host for each switch:

   host --- switch --- switch --- host

Adding the 'topos' dict with a key/value pair to generate our newly defined
topology enables one to pass in '--topo=mytopo' from the command line.
"""

from mininet.topo import Topo

class MyTopo( Topo ):
    "Simple topology example."

    def build( self ):
        "Create custom topo."

        # Add hosts and switches
        l1 = self.addHost( 'h1' )
        l2 = self.addHost( 'h2' )
        m1 = self.addHost( 'h3' )
        m2 = self.addHost( 'h4' )
        m3 = self.addHost( 'h5' )
        r1 = self.addHost( 'h6' )
        r2 = self.addHost( 'h7' )
        r3 = self.addHost( 'h8' )
        s1  = self.addSwitch( 's1' )
        s2 = self.addSwitch( 's2' )
        s3 = self.addSwitch( 's3' )

        # Add links
        self.addLink( l1, s1 )
        self.addLink( l2, s1 )
        self.addLink( s1, s2 )
        self.addLink( m1, s2 )
        self.addLink( m2, s2 )
        self.addLink( m3, s2 )
        self.addLink( s2, s3 )
        self.addLink( r1, s3 )
        self.addLink( r2, s3 )
        self.addLink( r3, s3 )


topos = { 'mytopo': ( lambda: MyTopo() ) }
