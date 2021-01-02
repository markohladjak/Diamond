R"~~~~(
<!doctype html>
<html lang="en">
  <head>
    <!-- Required meta tags -->
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">

    <!-- Bootstrap CSS -->
    <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@4.5.3/dist/css/bootstrap.min.css"
         integrity="sha384-TX8t27EcRE3e/ihU7zmQxVncDAy5uIKz4rEkgIXeMed4M0jlfIDPvg6uqKI2xXr2" crossorigin="anonymous">
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
    <title>Diamond</title>
    
  </head>
  <body>
<div class="container">

        <nav class="navbar navbar-dark bg-dark">
            <span class="navbar-brand mb-0 h1">TOYOTA</span>
        </nav>

        <div class="accordion" id="accordionExample">
            <div class="card">
              <div class="card-header" id="headingOne">
                <h2 class="mb-0">
                  <button class="btn btn-link btn-block text-left" type="button" data-toggle="collapse" data-target="#collapseOne" aria-expanded="true" aria-controls="collapseOne">
                    Список
                  </button>
                </h2>
              </div>
          
              <div id="collapseOne" class="collapse show" aria-labelledby="headingOne" data-parent="#accordionExample">
                <div class="card-body">


                    <table class="table table-striped table-dark">
                        <thead>
                          <tr>
                            <th scope="col">#</th>
                            <th scope="col">Стан</th>
                            <th scope="col">Блок</th>
                          </tr>
                        </thead>
                        <tbody id='tableBody'>
                          
                        </tbody>
                      </table>
                </div>
              </div>
            </div>
        </div>
</div>
              

            


           





    <!-- Optional JavaScript; choose one of the two! -->

    <!-- Option 1: jQuery and Bootstrap Bundle (includes Popper) -->
    <script src="https://code.jquery.com/jquery-3.5.1.slim.min.js" integrity="sha384-DfXdz2htPH0lsSSs5nCTpuj/zy4C+OGpamoFVy38MVBnE+IbbVYUew+OrCXaRkfj" crossorigin="anonymous"></script>
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@4.5.3/dist/js/bootstrap.bundle.min.js" integrity="sha384-ho+j7jyWK8fNQe+A12Hb8AhRq26LrZ/JpcUGGOn+Y7RsweNrtN/tE3MoK7ZeZDyx" crossorigin="anonymous"></script>

    <!-- Option 2: jQuery, Popper.js, and Bootstrap JS
    <script src="https://code.jquery.com/jquery-3.5.1.slim.min.js" integrity="sha384-DfXdz2htPH0lsSSs5nCTpuj/zy4C+OGpamoFVy38MVBnE+IbbVYUew+OrCXaRkfj" crossorigin="anonymous"></script>
    <script src="https://cdn.djsdelivr.net/npm/popper.js@1.16.1/dist/umd/popper.min.js" integrity="sha384-9/reFTGAW83EW2RDu2S0VKaIzap3H66lZH81PoYlFhbGU+6BZp6G7niu735Sk7lN" crossorigin="anonymous"></script>
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@4.5.3/dist/js/bootstrap.min.js" integrity="sha384-w1Q4orYjBQndcko6MimVbzY0tgp4pWB4lZ7lr30WKz0vr/aWKhXdBNmNb5D92v7s" crossorigin="anonymous"></script>
    -->
      
    <script>
      
        
        var disabledRows = [];
        var rows = [
            {
            id: 1,
            disabled: false,
            currentState: 'free'
        },
        {
            id: 2,
            disabled: false,
            currentState: 'free'

        },
        {
            id: 3,
            disabled: false,
            currentState: 'free'

        },
        {
            id: 4,
            disabled: false,
            currentState: 'free'
        }
        ,
        {
            id: 5,
            disabled: false,
            currentState: 'free'
        }
        ];

        function onWork (id)  {
            setCurrentState(id, 'work')
            updateBody()
            }
        function onDone (id)  {
            setCurrentState(id, 'done')
            updateBody()
            }

        function onFree (id)  {
            setCurrentState(id, 'free')
            updateBody()
            }

        function onSos (id)  {
            setCurrentState(id, 'sos')
            updateBody()
            playAudio()
            }

        function toggleActive (id)  {
            console.log(isRowDisabled(id));
             if(isRowDisabled(id)){
                 disabledRows = disabledRows.filter(el => el != id);
             } else {
                 disabledRows.push(id)
             }
             updateBody()
        }


        function isRowDisabled(id) {
            return disabledRows.some(el => el === id);
        }

        function  setCurrentState(id, state) {
            var row =  rows.filter(el => el.id === id)[0];
            var index = rows.indexOf(row);
            rows[index].currentState = state;
        }

        function updateBody(){ 
            // $( "#tableBody" ).empty();
            document.getElementById("tableBody").innerHTML = '';
            rows.forEach(renderRows);
        }

        function renderRows(item, index) {
            document.getElementById("tableBody").innerHTML +=
          `<tr>
                            <th scope="row">${item.id}</th>
                            <td>
                                <a href="#readData">
                                    <button class="badge  ${item.currentState === 'work'? 'btn-danger' : ''}" type="button"  id="work#${item.id}" ${isRowDisabled(item.id) ? 'disabled': ''}  onClick="onWork(${item.id})">Робота</button>
                                </a>
                                
                                <button class="badge  ${item.currentState === 'done' ? 'btn-success' : ''}" type="button" id="done#${item.id}" ${isRowDisabled(item.id) ? 'disabled': ''} onClick="onDone(${item.id})">Готово</button>
                                <button class="badge  ${item.currentState === 'free' ? 'btn-success' : ''}" type="button" id="free#${item.id}" ${isRowDisabled(item.id) ? 'disabled': ''} onClick="onFree(${item.id})">Вільно</button>
                                <button class="badge button  ${item.currentState === 'sos'? 'btn-danger'  : ''}" type="button"   id="sos#${item.id}"  ${isRowDisabled(item.id) ? 'disabled': ''} onClick="onSos(${item.id})">Sos</button>
                            </td>
                            <td>
                                <button class="badge " type="button" onClick="toggleActive(${item.id})">Вкл.</button>
                            </td>
                          </tr>`
                          
        }

        rows.forEach(renderRows);

    </script>
  </body>
</html>
)~~~~"