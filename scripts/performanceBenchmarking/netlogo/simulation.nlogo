extensions [ table ]

globals [
  adjacency
  weights
  node-values
  all-nodes
  log-file
]

turtles-own [
  val
  new-val
]

to setup
  clear-all
  set log-file "log.txt"
  file-open log-file
  file-print (word "=== Run started at " date-and-time " ===")
  file-close

  ;; Full absolute paths to graph and value folders
  let graph-folder "/home/josura/Projects/ccc/MASFENON/scripts/performanceBenchmarking/syntheticDataCreation/syntheticGraphsDifferentNodes/1000Nodes/graphs"
  let value-folder "/home/josura/Projects/ccc/MASFENON/scripts/performanceBenchmarking/syntheticDataCreation/syntheticGraphsDifferentNodes/1000Nodes/node_conditions_discr"

  let graph-files file-list graph-folder
  foreach graph-files [ full-graph-path ->
    let base-name file-name full-graph-path
    let full-value-path word value-folder "/" base-name
    process-graph-pair full-graph-path full-value-path base-name
  ]
end

to process-graph-pair [edge-path value-path base-name]
  file-close-all

  set adjacency table:make
  set weights table:make
  set node-values table:make
  set all-nodes []

  let runtime (java.lang.Runtime:getRuntime)
  reset-timer

  ;; load edges and node values
  load-graph edge-path
  load-node-values value-path

  ;; create turtles
  create-turtles length all-nodes [
    let node-id item who all-nodes
    set label node-id
    set val table:get node-values node-id
  ]

  let mem-before (runtime:totalMemory - runtime:freeMemory) / (1024 * 1024)
  propagate 0.1 50
  let elapsed-time timer
  let mem-after (runtime:totalMemory - runtime:freeMemory) / (1024 * 1024)

  ;; log results
  file-open log-file
  file-print (word "File: " base-name)
  file-print (word "  Time: " elapsed-time " sec")
  file-print (word "  Memory before: " mem-before " MB")
  file-print (word "  Memory after : " mem-after " MB")
  file-close
end

to load-graph [path]
  file-open path
  while [not file-at-end?] [
    let line file-read-line
    if not empty? line [
      let parts but-first remove "" split line
      let from item 0 parts
      let to item 1 parts
      let w read-from-string item 2 parts

      if not member? from all-nodes [ set all-nodes lput from all-nodes ]
      if not member? to all-nodes [ set all-nodes lput to all-nodes ]

      let neighbors table:get adjacency from []
      table:put adjacency from lput to neighbors

      table:put weights (word from "-" to) w
    ]
  ]
  file-close
end

to load-node-values [path]
  file-open path
  while [not file-at-end?] [
    let line file-read-line
    if not empty? line [
      let parts but-first remove "" split line
      let id item 0 parts
      let val read-from-string item 1 parts
      table:put node-values id val
    ]
  ]
  file-close
end

to propagate [scaling steps]
  ask turtles [
    set val table:get node-values label
  ]

  repeat steps [
    ask turtles [ set new-val 0 ]
    ask turtles [
      let from label
      let neighbors table:get adjacency from []
      foreach neighbors [ to-id ->
        let w table:get weights (word from "-" to-id)
        ask turtle (position to-id all-nodes) [
          set new-val new-val + scaling * w * [val] of turtle (position from all-nodes)
        ]
      ]
    ]
    ask turtles [ set val new-val ]
  ]

  ;; optionally store final values back
  ask turtles [
    table:put node-values label val
  ]
end
