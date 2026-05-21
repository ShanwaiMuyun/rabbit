import { createRouter, createWebHistory } from 'vue-router'
import Login from '@/views/Login/index.vue'
import Layout from '@/views/Layout/index.vue'
import Home from '@/views/Home/index.vue'
import Category from '@/views/Category/index.vue'
import SubCategory from '@/views/SubCategory/index.vue'
import Detail from '@/views/Detail/index.vue'
import CartList from '@/views/CartList/index.vue'
import Checkout from '@/views/Checkout/index.vue'
import Pay from '@/views/Pay/index.vue'
import Register from '@/views/Register/index.vue'
import AdminReview from '@/views/AdminReview/index.vue'
import SellerProducts from '@/views/SellerProducts/index.vue'
import OrderList from '@/views/OrderList/index.vue'
import Search from '@/views/Search/index.vue'
import Messages from '@/views/Messages/index.vue'

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes: [
    // 一级路由
    {
      path: '/',
      component: Layout,
      // 二级路由
      children: [
        {
          path: '',// 小技巧：当path为''时，表示该路由为默认路由，即访问http://localhost:8080/时，会自动跳转到http://localhost:8080/#/home
          component: Home,
        },
        {
          path: 'category/:id',
          component: Category,
        },
        // Category 和 SubCategory 在 Layout 组件中只能显示其中一个，因此不是放在 category 路由的 children 数组中
        {
          path: 'category/sub/:id',
          component: SubCategory,
        },
        {
          path: 'detail/:id',
          component: Detail,
        },
        {
          path: 'cartlist',
          component: CartList,
        },
        {
          path: 'checkout',
          component: Checkout,
        },
        {
          path: 'pay',
          component: Pay,
        },
        {
          path: 'admin/review',
          component: AdminReview,
        },
        {
          path: 'seller/products',
          component: SellerProducts,
        },
        {
          path: 'orders',
          component: OrderList,
        },
        {
          path: 'search',
          component: Search,
        },
        {
          path: 'messages',
          component: Messages,
        }
      ]
    },
    {
      path: '/login',
      component: Login,
    },
    {
      path: '/register',
      component: Register,
    }
  ],
  // 路由滚动行为定制
  scrollBehavior() {
    return {
      top: 0
    }
  }
})

export default router
